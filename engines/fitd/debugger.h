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

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#ifdef INTERNAL_DEBUGGER

////// debug var used in engine
extern bool debuggerVar_drawModelZv;
extern bool debuggerVar_drawCameraCoverZone;
extern bool debuggerVar_noHardClip;
extern bool debuggerVar_topCamera;
extern long int debufferVar_topCameraZoom;

extern bool debuggerVar_useBlackBG;
///////////////////////////////

void debugger_enterMainDebug(void);
void debugger_processDebugMenu(void);
void debugger_draw(void);
#endif // INTERNAL_DEBUGGER

#endif
