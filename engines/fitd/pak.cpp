/* Residual - A 3D game interpreter
 *
 * Residual is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

// seg 55

#include "common/endian.h"
#include "common/file.h"
#include "pak.h"
#include "unpack.h"

#ifdef _DEBUG
#define ASSERT(exp) assert(exp)
#else
#define ASSERT(exp)
#endif

typedef struct pakInfoStruct // warning: allignement unsafe
{
	long int discSize;
	long int uncompressedSize;
	char compressionFlag;
	char info5;
	short int offset;
};

typedef struct pakInfoStruct pakInfoStruct;

//#define USE_UNPACKED_DATA

void readPakInfo(pakInfoStruct* pPakInfo, Common::File *file)
{
	pPakInfo->discSize = file->readUint32LE();
	pPakInfo->uncompressedSize = file->readUint32LE();
	pPakInfo->compressionFlag = file->readByte();
	//	fread(&pPakInfo->compressionFlag,1,1,fileHandle);
	pPakInfo->info5 = file->readByte();
	//fread(&pPakInfo->info5,1,1,fileHandle);
	pPakInfo->offset = file->readUint16LE();
	//fread(&pPakInfo->offset,2,1,fileHandle);
	
/*	pPakInfo->discSize = READ_LE_U32(&pPakInfo->discSize);
	pPakInfo->uncompressedSize = READ_LE_U32(&pPakInfo->uncompressedSize);
	pPakInfo->offset = READ_LE_U16(&pPakInfo->offset);*/
}

unsigned int PAK_getNumFiles(char* name)
{
	char bufferName[256];
	Common::File *fileHandle;
	long int fileOffset;
	
	strcpy(bufferName, name); // temporary until makeExtention is coded
	strcat(bufferName,".PAK");
	
	fileHandle = new Common::File();
	fileHandle->open(bufferName);
	
	ASSERT(fileHandle);
	
	fileHandle->seek(4,SEEK_CUR);
	fileOffset = fileHandle->readUint32LE();

	delete fileHandle;
	
	return((fileOffset/4)-2);
}

int loadPakToPtr(char* name, int index, char* ptr)
{
#ifdef USE_UNPACKED_DATA
	char buffer[256];
	FILE* fHandle;
	int size;
	
	sprintf(buffer,"%s/%04X.OUT",name,index);
	
	fHandle = fopen(buffer,"rb");
	
	if(!fHandle)
		return(0);
	
	fseek(fHandle,0L,SEEK_END);
	size = ftell(fHandle);
	fseek(fHandle,0L,SEEK_SET);
	
	fread(ptr,size,1,fHandle);
	fclose(fHandle);
	
	return 1;
#else
	char* lptr;
	
	lptr = loadPak(name,index);
	
	memcpy(ptr,lptr,getPakSize(name,index));
	
	free(lptr);
	
	return 1;
#endif
}

int getPakSize(char* name, int index)
{
#ifdef USE_UNPACKED_DATA
	char buffer[256];
	FILE* fHandle;
	int size;
	
	sprintf(buffer,"%s/%04X.OUT",name,index);
	
	fHandle = fopen(buffer,"rb");
	
	if(!fHandle)
		return(0);
	
	fseek(fHandle,0L,SEEK_END);
	size = ftell(fHandle);
	fseek(fHandle,0L,SEEK_SET);
	
	fclose(fHandle);
	
	return (size);
#else
	char bufferName[256];
	Common::File *fileHandle;
	long int fileOffset;
	long int additionalDescriptorSize;
	pakInfoStruct pakInfo;
	char* ptr=0;
	long int size=0;
	
	strcpy(bufferName, name); // temporary until makeExtention is coded
	strcat(bufferName,".PAK");
	
	fileHandle = new Common::File();
		
	if(fileHandle->open(bufferName)) // a bit stupid, should return NULL right away
	{
		fileHandle->seek((index+1)*4,SEEK_SET);
		fileOffset = fileHandle->readUint32LE();

		fileHandle->seek(fileOffset,SEEK_SET);
		
		additionalDescriptorSize = fileHandle->readUint32LE();
		
		readPakInfo(&pakInfo,fileHandle);
		
		fileHandle->seek(pakInfo.offset,SEEK_CUR);
		
		if(pakInfo.compressionFlag == 0) // uncompressed
		{
			size = pakInfo.discSize;
		}
		else if(pakInfo.compressionFlag == 1) // compressed
		{
			size = pakInfo.uncompressedSize;
		}
		else if(pakInfo.compressionFlag == 4)
		{
			size = pakInfo.uncompressedSize;
		}
		delete fileHandle;
	}
	
	return size;
#endif
}

char* loadPak(char* name, int index)
{
#ifdef USE_UNPACKED_DATA
	char buffer[256];
	FILE* fHandle;
	int size;
	char* ptr;
	
	sprintf(buffer,"%s/%04X.OUT",name,index);
	
	fHandle = fopen(buffer,"rb");
	
	if(!fHandle)
		return NULL;
	
	fseek(fHandle,0L,SEEK_END);
	size = ftell(fHandle);
	fseek(fHandle,0L,SEEK_SET);
	
	ptr = (char*)malloc(size);
	
	fread(ptr,size,1,fHandle);
	fclose(fHandle);
	
	return ptr;
#else
	char bufferName[256];
	//FILE* fileHandle;
	Common::File *fileHandle;
	long int fileOffset;
	long int additionalDescriptorSize;
	pakInfoStruct pakInfo;
	char* ptr=0;
	
	//makeExtention(bufferName, name, ".PAK");
	strcpy(bufferName, name); // temporary until makeExtention is coded
	strcat(bufferName,".PAK");
	
	//fileHandle = fopen(bufferName,"rb");
	fileHandle = new Common::File();
	
	if(fileHandle->open(bufferName)) // a bit stupid, should return NULL right away
	{
		char nameBuffer[256];
		
		fileHandle->seek((index+1)*4,SEEK_SET);
		//	fseek(fileHandle,(index+1)*4,SEEK_SET);
		// TODO endian-verification
		fileOffset = fileHandle->readUint32LE();
		//fread(&fileOffset,4,1,fileHandle);
		
/*#ifdef MACOSX
		fileOffset = READ_LE_U32(&fileOffset);
#endif*/
		
		fileHandle->seek(fileOffset,SEEK_SET);

		//fseek(fileHandle,fileOffset,SEEK_SET);
		additionalDescriptorSize = fileHandle->readUint32LE();
		//fread(&additionalDescriptorSize,4,1,fileHandle);
		
/*#ifdef MACOSX
		additionalDescriptorSize = READ_LE_U32(&additionalDescriptorSize);
#endif*/
		
		readPakInfo(&pakInfo,fileHandle);
		
		if(pakInfo.offset)
		{
			ASSERT(pakInfo.offset < 256);
			
			fileHandle->read(nameBuffer,pakInfo.offset);
//			fread(nameBuffer,pakInfo.offset,1,fileHandle);
#ifdef INTERNAL_DEBUGGER
			printf("Loading %s/%s\n", name,nameBuffer+2);
#endif
		}
		else
		{
			fileHandle->seek(pakInfo.offset,SEEK_CUR);
		}
		
		switch(pakInfo.compressionFlag)
		{
			case 0:
			{
				ptr = new char[pakInfo.discSize];
				fileHandle->read(ptr,pakInfo.discSize);
				break;
			}
			case 1:
			{
				char * compressedDataPtr = new char[pakInfo.discSize];
				fileHandle->read(compressedDataPtr, pakInfo.discSize);
				//				fread(compressedDataPtr, pakInfo.discSize, 1, fileHandle);
				ptr = new char[pakInfo.uncompressedSize];
				
				PAK_explode((unsigned char*)compressedDataPtr, (unsigned char*)ptr, pakInfo.discSize, pakInfo.uncompressedSize, pakInfo.info5);
				
				delete[] compressedDataPtr;
				break;
			}
			case 4:
			{
				char * compressedDataPtr = new char[pakInfo.discSize];
				fileHandle->read(compressedDataPtr, pakInfo.discSize);
				//				fread(compressedDataPtr, pakInfo.discSize, 1, fileHandle);
				ptr = new char[pakInfo.uncompressedSize];
				
				PAK_deflate((unsigned char*)compressedDataPtr, (unsigned char*)ptr, pakInfo.discSize, pakInfo.uncompressedSize);
				
				delete[] compressedDataPtr;
				break;
			}
		}
		delete fileHandle;
	}
	
	return ptr;
#endif
}
