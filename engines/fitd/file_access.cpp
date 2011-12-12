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

#include "common.h"
#include "common/textconsole.h"
#include "common/file.h"

// seg 20
void theEnd(int type, char* name)
{
	//  freeScene();
	freeAll();
	error("TheEnd: %s", name);
	//exit(1);
}
char* loadFromItd(char* name)
{
	Common::File *fHandle;
	char* ptr;
	fHandle = new Common::File();

	if(!fHandle->open(name))
	{
		theEnd(0,name);
		return NULL;
	}

	fileSize = fHandle->size();
	ptr = new char[fileSize];
	
	if(!ptr)
	{
		theEnd(1,name);
		return NULL;
	}
	fHandle->read(ptr, fileSize);

	delete fHandle;
	return(ptr);
}

char* loadPakSafe(char* name, int index)
{
	char* ptr;
	ptr = loadPak(name, index);
	if(!ptr)
	{
		theEnd(0,name);
	}
	return ptr;
}
