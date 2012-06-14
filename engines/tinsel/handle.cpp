/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/tinsel/handle.cpp $
 * $Id: handle.cpp 40878 2009-05-25 07:42:01Z Hkz $
 *
 * This file contains the handle based Memory Manager code
 */

#define BODGE

#include "common/file.h"

#include "tinsel/drives.h"
#include "tinsel/dw.h"
#include "tinsel/scn.h"			// name of "index" file
#include "tinsel/handle.h"
#include "tinsel/heapmem.h"			// heap memory manager


// these are included only so the relevant structs can be used in convertLEStructToNative()
#include "tinsel/anim.h"
#include "tinsel/multiobj.h"
#include "tinsel/film.h"
#include "tinsel/object.h"
#include "tinsel/palette.h"
#include "tinsel/text.h"
#include "tinsel/timers.h"
#include "tinsel/tinsel.h"
#include "tinsel/scene.h"

namespace Tinsel {

//----------------- EXTERNAL GLOBAL DATA --------------------

#ifdef DEBUG
bool bLockedScene = 0;
#endif


//----------------- LOCAL DEFINES --------------------

struct MEMHANDLE {
	char szName[12];	//!< 00 - file name of graphics file
	int32 filesize;		//!< 12 - file size and flags
	MEM_NODE *pNode;	//!< 16 - memory node for the graphics
	uint32 flags2;
};


/** memory allocation flags - stored in the top bits of the filesize field */
enum {
	fPreload	= 0x01000000L,	//!< preload memory
	fDiscard	= 0x02000000L,	//!< discard memory
	fSound		= 0x04000000L,	//!< sound data
	fGraphic	= 0x08000000L,	//!< graphic data
	fCompressed	= 0x10000000L,	//!< compressed data
	fLoaded		= 0x20000000L	//!< set when file data has been loaded
};
#define	FSIZE_MASK	(TinselV3 ? 0xFFFFFFFFL : 0x00FFFFFFL)	//!< mask to isolate the filesize
#define	MALLOC_MASK	(TinselV3 ? 0L : 0xFF000000L)	//!< mask to isolate the memory allocation flags
//#define	HANDLEMASK		0xFF800000L	//!< get handle of address

#define MEMFLAGS(x) (TinselV3 ? x->flags2 : x->filesize)
#define MEMFLAGSET(x, mask) (TinselV3 ? x->flags2 |= mask : x->filesize |= mask)

//----------------- LOCAL GLOBAL DATA --------------------

// handle table gets loaded from index file at runtime
static MEMHANDLE *handleTable = 0;

// number of handles in the handle table
static uint numHandles = 0;

static uint32 cdPlayHandle = (uint32)-1;

static int	cdPlayFileNum, cdPlaySceneNum;
static SCNHANDLE cdBaseHandle = 0, cdTopHandle = 0;
static Common::File cdGraphStream;

static char szCdPlayFile[100];

//----------------- FORWARD REFERENCES --------------------

static void LoadFile(MEMHANDLE *pH, bool bWarn);	// load a memory block as a file

struct decode_t {
	Common::File *hFile;
	short bitmask;
	short charbuff;
	int remaining;
};

static char lzss_window[4096];

bool LZSS_GetBit(decode_t &input) {
	if(input.bitmask == 0x80) {
		if(input.remaining == 0) {
			input.remaining -= 1;
			input.bitmask = -1;
			return 0;
		}
		
		input.remaining -= 1;
		input.charbuff = (short)input.hFile->readByte();
	} 

	bool mybit = (input.charbuff & input.bitmask);
	input.bitmask >>= 1;
	if(input.bitmask == 0) input.bitmask = 0x80;
	
	return mybit;
}

int LZSS_GetBits(decode_t &input, int count) {
	count--;
	short bitmask = 1 << count; //esi
	int returnval = 0; //edi
	
	while(bitmask != 0) {
		if(input.bitmask == 0x80) {
			if(input.remaining == 0) {
				input.remaining -= 1;
				input.charbuff = -1;
				return 0;
			} 
			input.remaining -= 1;
			input.charbuff = (short)input.hFile->readByte();
		}
		
		if(input.bitmask & input.charbuff) returnval |= bitmask;
		input.bitmask = input.bitmask >> 1;
		bitmask >>= 1;

		if(input.bitmask == 0) input.bitmask = 0x80;
	}
	return returnval;
}

int LZSS_Decode(decode_t &input, char *output) {
	int offset = 0; //esi
	int code_offset, code_length;
	int nextChar = 1;
	
	memset(lzss_window, 0, 4096);
	
	while(1) {
		if(LZSS_GetBit(input)) { //Straight byte
			char a = (char)LZSS_GetBits(input, 8);
			lzss_window[nextChar] = a;
			output[offset] = a;
			offset++;
			nextChar += 1;
			nextChar &= 0xFFF;
		} else { //Pointer
			code_offset = LZSS_GetBits(input, 12);
			if(code_offset == 0) break;

			code_length = LZSS_GetBits(input, 4) + 1;
			for(int i=0; i <= code_length; i++) {
				char c = lzss_window[code_offset+i];
				output[offset++] = c;
				lzss_window[nextChar] = c;
				nextChar += 1;
				nextChar &= 0xFFF;
			}			
		}
	};
	
	return offset;
}



/**
 * Loads the graphics handle table index file and preloads all the
 * permanent graphics etc.
 */
void SetupHandleTable(void) {
	bool t2Flag = TinselV2;
	int RECORD_SIZE = t2Flag ? 24 : 20;

	int len;
	uint i;
	MEMHANDLE *pH;
	Common::File f;

	if (f.open(TinselV1PSX? PSX_INDEX_FILENAME : INDEX_FILENAME)) {
		// get size of index file
		len = f.size();

		if (len > 0) {
			if ((len % RECORD_SIZE) != 0) {
				// index file is corrupt
				error(FILE_IS_CORRUPT, TinselV1PSX? PSX_INDEX_FILENAME : INDEX_FILENAME);
			}

			// calc number of handles
			numHandles = len / RECORD_SIZE;

			// allocate memory for the index file
			handleTable = (MEMHANDLE *)calloc(numHandles, sizeof(struct MEMHANDLE));

			// make sure memory allocated
			assert(handleTable);

			// load data
			for (i = 0; i < numHandles; i++) {
				f.read(handleTable[i].szName, 12);
				handleTable[i].filesize = f.readUint32LE();
				// The pointer should always be NULL. We don't
				// need to read that from the file.
				handleTable[i].pNode = NULL;
				f.seek(4, SEEK_CUR);
				// For Discworld 2, read in the flags2 field
				handleTable[i].flags2 = t2Flag ? f.readUint32LE() : 0;
			}

			if (f.ioFailed()) {
				// index file is corrupt
				error(FILE_IS_CORRUPT, (TinselV1PSX? PSX_INDEX_FILENAME : INDEX_FILENAME));
			}

			// close the file
			f.close();
		} else {	// index file is corrupt
			error(FILE_IS_CORRUPT, (TinselV1PSX? PSX_INDEX_FILENAME : INDEX_FILENAME));
		}
	} else {	// cannot find the index file
		error(CANNOT_FIND_FILE, (TinselV1PSX? PSX_INDEX_FILENAME : INDEX_FILENAME));
	}

	// allocate memory nodes and load all permanent graphics
	for (i = 0, pH = handleTable; i < numHandles; i++, pH++) {
		if (MEMFLAGS(pH) & fPreload) {
			// allocate a fixed memory node for permanent files
			pH->pNode = MemoryAlloc(DWM_FIXED, sizeof(MEM_NODE) + (pH->filesize & FSIZE_MASK));

			// make sure memory allocated
			assert(pH->pNode);

			// Initialise the MEM_NODE structure
			memset(pH->pNode, 0, sizeof(MEM_NODE));

			// load the data
			LoadFile(pH, true);
		}
#ifdef BODGE
		else if ((pH->filesize & FSIZE_MASK) == 8) {
			pH->pNode = NULL;
		}
#endif
		else {
			// allocate a discarded memory node for other files
			pH->pNode = MemoryAlloc(
				DWM_MOVEABLE | DWM_DISCARDABLE | DWM_NOALLOC,
				pH->filesize & FSIZE_MASK);

			// make sure memory allocated
			assert(pH->pNode);
		}
	}
}

void FreeHandleTable(void) {
	if (handleTable) {
		free(handleTable);
		handleTable = NULL;
	}
	if (cdGraphStream.isOpen())
		cdGraphStream.close();
}

/**
 * Loads a memory block as a file.
 */
void OpenCDGraphFile(void) {
	if (cdGraphStream.isOpen())
		cdGraphStream.close();

	// As the theory goes, the right CD will be in there!

	cdGraphStream.clearIOFailed();
	cdGraphStream.open(szCdPlayFile);
	if (cdGraphStream.ioFailed())
		error(CANNOT_FIND_FILE, szCdPlayFile);
}

void LoadCDGraphData(MEMHANDLE *pH) {
	// read the data
	uint bytes;
	byte *addr;
	int	retries = 0;

	assert(!(!TinselV3 && pH->filesize & fCompressed));

	// Can't be preloaded
	assert(!(MEMFLAGS(pH) & fPreload));

	// discardable - lock the memory
	addr = (byte *)MemoryLock(pH->pNode);

	// make sure address is valid
	assert(addr);

	// Move to correct place in file and load the required data
	cdGraphStream.seek(cdBaseHandle & OFFSETMASK, SEEK_SET);
	bytes = cdGraphStream.read(addr, (cdTopHandle - cdBaseHandle) & OFFSETMASK);

	// New code to try and handle CD read failures 24/2/97
	while (bytes != ((cdTopHandle - cdBaseHandle) & OFFSETMASK) && retries++ < MAX_READ_RETRIES)	{
		// Try again
		cdGraphStream.seek(cdBaseHandle & OFFSETMASK, SEEK_SET);
		bytes = cdGraphStream.read(addr, (cdTopHandle - cdBaseHandle) & OFFSETMASK);
	}

	// discardable - unlock the memory
	MemoryUnlock(pH->pNode);

	// set the loaded flag
	MEMFLAGSET(pH, fLoaded);

	// clear the loading flag
//	pH->filesize &= ~fLoading;

	if (bytes != ((cdTopHandle-cdBaseHandle) & OFFSETMASK))
		// file is corrupt
		error(FILE_READ_ERROR, "CD play file");
}

/**
 * Called immediatly preceding a CDplay().
 * Prepares the ground so that when LockMem() is called, the
 * appropriate section of the extra scene file is loaded.
 * @param start			Handle of start of range
 * @param next			Handle of end of range + 1
 */
void LoadExtraGraphData(SCNHANDLE start, SCNHANDLE next) {
	if (cdPlayFileNum == cdPlaySceneNum && start == cdBaseHandle)
		return;

	OpenCDGraphFile();

	if ((handleTable + cdPlayHandle)->pNode->pBaseAddr != NULL)
		MemoryDiscard((handleTable + cdPlayHandle)->pNode); // Free it

	// It must always be the same
	assert(cdPlayHandle == (start >> SCNHANDLE_SHIFT));
	assert(cdPlayHandle == (next >> SCNHANDLE_SHIFT));

	cdBaseHandle = start;
	cdTopHandle = next;
}

void SetCdPlaySceneDetails(int fileNum, const char *fileName) {
	cdPlaySceneNum = fileNum;
	strcpy(szCdPlayFile, fileName);
}

void SetCdPlayHandle(int fileNum) {
	cdPlayHandle = fileNum;
}


/**
 * Loads a memory block as a file.
 * @param pH			Memory block pointer
 * @param bWarn			If set, treat warnings as errors
 */
void LoadFile(MEMHANDLE *pH, bool bWarn) {
	Common::File f;
	char szFilename[sizeof(pH->szName) + 1];

	if (!TinselV3 && pH->filesize & fCompressed) {
		error("Compression handling has been removed!");
	}

	// extract and zero terminate the filename
	strncpy(szFilename, pH->szName, sizeof(pH->szName));
	szFilename[sizeof(pH->szName)] = 0;

	if (f.open(szFilename)) {
		// read the data
		int bytes;
		uint8 *addr;

		if (MEMFLAGS(pH) & fPreload)
			// preload - no need to lock the memory
			addr = (uint8 *)pH->pNode + sizeof(MEM_NODE);
		else {
			// discardable - lock the memory
			addr = (uint8 *)MemoryLock(pH->pNode);
		}
#ifdef DEBUG
		if (addr == NULL) {
			if (pH->filesize & fPreload)
				// preload - no need to lock the memory
				addr = (uint8 *)pH->pNode;
			else {
				// discardable - lock the memory
				addr = (uint8 *)MemoryLock(pH->pNode);
			}
		}
#endif

		// make sure address is valid
		assert(addr);

		if (TinselV3 && pH->flags2 & fCompressed) {
			decode_t decodeinfo;
			decodeinfo.hFile = &f;
			decodeinfo.bitmask = 0x80;
			decodeinfo.remaining = f.size();
			decodeinfo.charbuff = 0;
			bytes = LZSS_Decode(decodeinfo, (char *)addr);
		} else {
			bytes = f.read(addr, pH->filesize & FSIZE_MASK);
		}
		// close the file
		f.close();

		if ((MEMFLAGS(pH) & fPreload) == 0) {
			// discardable - unlock the memory
			MemoryUnlock(pH->pNode);
		}

		// set the loaded flag
		MEMFLAGSET(pH, fLoaded);

		if (bytes == (int)(pH->filesize & FSIZE_MASK)) {
			return;
		}

		if (bWarn)
			// file is corrupt
			error(FILE_IS_CORRUPT, szFilename);
	}

	if (bWarn)
		// cannot find file
		error(CANNOT_FIND_FILE, szFilename);
}

/**
 * Returns the address of a image, given its memory handle.
 * @param offset			Handle and offset to data
 */
byte *LockMem(SCNHANDLE offset) {
	uint32 handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use
	MEMHANDLE *pH;			// points to table entry

	// range check the memory handle
	assert(handle < numHandles);

	pH = handleTable + handle;

	if (MEMFLAGS(pH) & fPreload) {
		if (TinselV2)
			// update the LRU time (new in this file)
			pH->pNode->lruTime = DwGetCurrentTime();

		// permanent files are already loaded
		return (uint8 *)pH->pNode + sizeof(MEM_NODE) + (offset & OFFSETMASK);
	} else if (handle == cdPlayHandle) {
		// Must be in currently loaded/loadable range
		if (offset < cdBaseHandle || offset >= cdTopHandle)
			error("Overlapping (in time) CD-plays!");

		if (pH->pNode->pBaseAddr && (MEMFLAGS(pH) & fLoaded))
			// already allocated and loaded
			return pH->pNode->pBaseAddr + ((offset - cdBaseHandle) & OFFSETMASK);

		if (pH->pNode->pBaseAddr == NULL)
			// must have been discarded - reallocate the memory
			MemoryReAlloc(pH->pNode, cdTopHandle-cdBaseHandle,
				DWM_MOVEABLE | DWM_DISCARDABLE);

		if (pH->pNode->pBaseAddr == NULL)
			error("Out of memory");

		LoadCDGraphData(pH);

		// make sure address is valid
		assert(pH->pNode->pBaseAddr);

		// update the LRU time (new in this file)
		pH->pNode->lruTime = DwGetCurrentTime();

		return pH->pNode->pBaseAddr + ((offset - cdBaseHandle) & OFFSETMASK);

	} else {
		if (pH->pNode->pBaseAddr && (MEMFLAGS(pH) & fLoaded))
			// already allocated and loaded
			return pH->pNode->pBaseAddr + (offset & OFFSETMASK);

		if (pH->pNode->pBaseAddr == NULL)
			// must have been discarded - reallocate the memory
			MemoryReAlloc(pH->pNode, pH->filesize & FSIZE_MASK,
				DWM_MOVEABLE | DWM_DISCARDABLE);

		if (pH->pNode->pBaseAddr == NULL)
			error("Out of memory");

		if (TinselV2) {
			SetCD(pH->flags2 & fAllCds);
			CdCD(nullContext);
		}
		LoadFile(pH, true);

		// make sure address is valid
		assert(pH->pNode->pBaseAddr);

		return pH->pNode->pBaseAddr + (offset & OFFSETMASK);
	}
}

/**
 * Called to make the current scene non-discardable.
 * @param offset			Handle and offset to data
 */
void LockScene(SCNHANDLE offset) {

	uint32 handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use
	MEMHANDLE *pH;					// points to table entry

#ifdef DEBUG
	assert(!bLockedScene); // Trying to lock more than one scene
#endif

	// range check the memory handle
	assert(handle < numHandles);

	pH = handleTable + handle;

	// compact the heap to avoid fragmentation while scene is non-discardable
	HeapCompact(MAX_INT, false);

	if ((MEMFLAGS(pH) & fPreload) == 0) {
		// change the flags for the node
		// WORKAROUND: The original didn't include the DWM_LOCKED flag. It's being
		// included because the method is 'LockScene' so it's presumed that the
		// point of this was that the scene's memory block be locked
		MemoryReAlloc(pH->pNode, pH->filesize & FSIZE_MASK, DWM_MOVEABLE | DWM_LOCKED);
#ifdef DEBUG
		bLockedScene = true;
#endif
	}
}

/**
 * Called to make the current scene discardable again.
 * @param offset			Handle and offset to data
 */
void UnlockScene(SCNHANDLE offset) {

	uint32 handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use
	MEMHANDLE *pH;					// points to table entry

	// range check the memory handle
	assert(handle < numHandles);

	pH = handleTable + handle;

	if ((MEMFLAGS(pH) & fPreload) == 0) {
		// change the flags for the node
		MemoryReAlloc(pH->pNode, pH->filesize & FSIZE_MASK, DWM_MOVEABLE | DWM_DISCARDABLE);
#ifdef DEBUG
		bLockedScene = false;
#endif
	}
}

/*----------------------------------------------------------------------*/

#ifdef BODGE

/**
 * Validates that a specified handle pointer is valid
 * @param offset			Handle and offset to data
 */
bool ValidHandle(SCNHANDLE offset) {
	uint32 handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use
	MEMHANDLE *pH;					// points to table entry

	// range check the memory handle
	assert(handle < numHandles);

	pH = handleTable + handle;

	return (pH->filesize & FSIZE_MASK) != 8;
}
#endif

/**
 * TouchMem
 * @param offset			Handle and offset to data
 */
void TouchMem(SCNHANDLE offset) {
	MEMHANDLE *pH;					// points to table entry
	uint32 handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use

	if (offset != 0) {
		pH = handleTable + handle;

		// update the LRU time whether its loaded or not!
		pH->pNode->lruTime = DwGetCurrentTime();
	}
}

/**
 * Returns true if the given handle is into the cd graph data
 * @param offset			Handle and offset to data
 */
bool IsCdPlayHandle(SCNHANDLE offset) {
	uint32 handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use

	// range check the memory handle
	assert(handle < numHandles);

	return (handle == cdPlayHandle);
}

/**
 * Returns the CD for a given scene handle
 */
int CdNumber(SCNHANDLE offset) {
	uint handle = offset >> SCNHANDLE_SHIFT;	// calc memory handle to use

	// range check the memory handle
	assert(handle < numHandles);

	MEMHANDLE *pH = handleTable + handle;

	if (!TinselV2)
		return 1;

	return GetCD(pH->flags2 & fAllCds);
}

int GetHandleIndex(char *name) {
	for(uint i=0; i < numHandles; i++) {
		if(!strncmp(name, handleTable[i].szName, 12)) {
			return i;
		}
	}
	return -1;
}

bool InMemory(SCNHANDLE offset) {
	uint handle = offset >> SCNHANDLE_SHIFT;

	assert(handle < numHandles);

	MEMHANDLE *pH = handleTable + handle;

	if(pH->flags2 & fPreload || pH->flags2 & fLoaded)
		return true;
	else
		return false;
}

} // end of namespace Tinsel
