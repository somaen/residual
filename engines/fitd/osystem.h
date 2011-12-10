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

#ifndef _OSYSTEM_H_
#define _OSYSTEM_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef INTERNAL_DEBUGGER
#include "debugFont.h"
#endif

#define byte char

/*
class OSystem
{
  public:
  */
    void osystem_init();

    extern int osystem_mouseRight;
    extern int osystem_mouseLeft;

    void osystem_delay(int time);
    void osystem_crossFade(char *buffer, char *palette);

  	void osystem_playSampleFromName(char* sampleName);
  void osystem_playSample(char* samplePtr,int size);
//    void getMouseStatus(mouseStatusStruct * mouseData);


/*
  private:
};*/

#endif
