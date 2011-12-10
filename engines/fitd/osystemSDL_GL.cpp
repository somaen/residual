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

#undef USE_GL
#ifdef USE_GL

/***************************************************************************
                          mainSDL.cpp  -  description
                             -------------------
    begin                : Mon Jun 3 2002
    copyright            : (C) 2002 by Yaz0r
    email                : yaz0r@yaz0r.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#include "SDL.h"
//#include "SDL_thread.h"
//#include "SDL_mixer.h"
#include "osystem.h"

#if defined (SDL_BACKEND) && !defined(__amigaos4__)
#include <SDL_opengl.h>
#undef ARRAYSIZE
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif



void osystem_updateImage()
{
}

/*void OSystem::getMouseStatus(mouseStatusStruct * mouseData)
{

    SDL_GetMouseState(&mouseData->X, &mouseData->Y);

    mouseData->left = mouseLeft;
    mouseData->right = mouseRight;

    mouseLeft = 0;
    mouseRight = 0;
}*/



void OPL_musicPlayer(void *udata, Uint8 *stream, int len)
{
	musicUpdate(udata,stream,len);
}

int tesselatePosition = 0;

void osystem_startBgPoly()
{
 // glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glBindTexture(GL_TEXTURE_2D, backTexture);
  //glBegin(GL_POLYGON);

  gluTessBeginPolygon(tobj, NULL);
  gluTessBeginContour(tobj);

  tesselatePosition = 0;
}

void osystem_endBgPoly()
{
  gluTessEndContour(tobj);
  gluTessEndPolygon(tobj);

 // glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glBindTexture(GL_TEXTURE_2D, 0);

}

void osystem_addBgPolyPoint(int x, int y)
{
  tesselateList[tesselatePosition][0] = x;
  tesselateList[tesselatePosition][1] = y;
  tesselateList[tesselatePosition][2] = 0;
  tesselateList[tesselatePosition][3] = 1.f;
  tesselateList[tesselatePosition][4] = 1.f;
  tesselateList[tesselatePosition][5] = 1.f;

  gluTessVertex(tobj, tesselateList[tesselatePosition], tesselateList[tesselatePosition]); 

  tesselatePosition++;
}


void osystem_stopFrame()
{
}

void osystem_startModelRender()
{
  //glNewList(modelsDisplayList, GL_COMPILE);
}

void osystem_stopModelRender()
{
  //glEndList();
}



#ifdef INTERNAL_DEBUGGER
void osystem_drawDebugText(const u32 X, const u32 Y, const u8* string)
{
#if 0
  u32 currentX = X;
  u32 i;
  u32 stringLength;

  glBindTexture(GL_TEXTURE_2D, debugFontTexture);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glColor4ub(255,255,255,255);

  stringLength = strlen(string);

  for(i=0;i<stringLength;i++)
  {
    float textX1;
    float textY1;
    float textX2;
    float textY2;

    u8 lineNumber;
    u8 colNumber;

    lineNumber = string[i]>>4;
    colNumber = string[i]&0xF;

    textX1 = ((256.f/16.f)*colNumber)/256.f;
    textY1 = ((256.f/16.f)*lineNumber)/256.f;
    textX2 = ((256.f/16.f)*(colNumber+1))/256.f;
    textY2 = ((256.f/16.f)*(lineNumber+1))/256.f;

    glBegin(GL_QUADS);

    glTexCoord2f(textX1,textY1);
    glVertex2d(currentX,Y);

    glTexCoord2f(textX2,textY1);
    glVertex2d(currentX+8,Y);

    glTexCoord2f(textX2,textY2);
    glVertex2d(currentX+8,Y+8);

    glTexCoord2f(textX1,textY2);
    glVertex2d(currentX,Y+8);

    glEnd();

    currentX+=4;
  }

  glEnable(GL_DEPTH_TEST);
#endif
}
#endif

#endif
