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
//#include <SDL.h>
#include "common/events.h"
#include "common/textconsole.h"

extern float nearVal;
extern float farVal;
extern float cameraZoom;
extern float fov;

void handleKey(int operation, int keyVarMagic, int keyModifier, uint16 ascii) {
	//SDL_Event event;
	int size;
	int j;
	unsigned char *keyboard;
	
	inputKey = 0;
	input1 = 0;
	input2 = 0;
	j = ascii;
	warning("Compare to: %d", Common::KEYCODE_UP);
	warning("Key %d , %d, %d pressed\n", keyVarMagic, j, keyModifier);
	/*while( SDL_PollEvent( &event ) ) {
		
		switch( event.type ) {
			case SDL_QUIT:
				cleanupAndExit();
				break;
		}
		
	}*/
	
	//keyboard = SDL_GetKeyState(&size);
	
/*	for (int j = 0; j < size; j++)
	{*/
	if (operation == Common::EVENT_KEYDOWN)
		{
			switch (keyVarMagic)
			{
			/*	case SDLK_z:
					nearVal-=1;
					break;
				case SDLK_x:
					nearVal+=1;
					break;
				case SDLK_a:
					cameraZoom-=100;
					break;
				case SDLK_s:
					cameraZoom+=100;
					break;
				case SDLK_w:
				{
					float factor = (float)cameraY/(float)cameraZ;
					cameraY+=1;
					cameraZ=(float)cameraY/factor;
					break;
				}
				case SDLK_q:
				{
					float factor = (float)cameraY/(float)cameraZ;
					cameraY-=1;
					cameraZ=(float)cameraY/factor;
					break;
				}
				case SDLK_e:
					fov-=1;
					break;
				case SDLK_r:
					fov+=2;
					break; */
				case Common::KEYCODE_RETURN:
					input2 = 0x1C;
					break;
				case Common::KEYCODE_ESCAPE:
					input2 = 0x1B;
					break;
					
				case Common::KEYCODE_UP:
					warning("UP");
					inputKey |= 1;
					break;
					
				case Common::KEYCODE_DOWN:
					inputKey |= 2;
					break;
					
				case Common::KEYCODE_RIGHT:
					inputKey |= 8;
					break;
					
				case Common::KEYCODE_LEFT:
					inputKey |= 4;
					break;
				case Common::KEYCODE_SPACE:
					input1 = 1;
					break;
#ifdef INTERNAL_DEBUGGER
				case Common::KEYCODE_o:
					debufferVar_topCameraZoom+=100;
					break;
				case SDLK_p:
					debufferVar_topCameraZoom-=100;
					break;
				case SDLK_t:
					debuggerVar_topCamera = !debuggerVar_topCamera;
					
					if(!debuggerVar_topCamera)
					{
						backgroundMode = backgroundModeEnum_2D;
					}
					break;
				case SDLK_c:
					debuggerVar_noHardClip = !debuggerVar_noHardClip;
					break;
				case SDLK_d:
					debugger_enterMainDebug();
					break;
				case SDLK_b:
					backgroundMode = backgroundModeEnum_3D;
					break;
				case SDLK_n:
					backgroundMode = backgroundModeEnum_2D;
					break;
#endif
				default:
					warning("Unhandled keycode");
					break;
			}
		}
#ifdef INTERNAL_DEBUGGER
	if(debuggerVar_topCamera)
	{
		backgroundMode = backgroundModeEnum_3D;
	}
#endif
	
}

void readKeyboard(void)
{
	inputKey = 0;
	process_events();
/*
  SDL_Event event;
  int size;
  int j;
  unsigned char *keyboard;

  inputKey = 0;
  input1 = 0;
  input2 = 0;

  while( SDL_PollEvent( &event ) ) {

    switch( event.type ) {
    case SDL_QUIT:
        cleanupAndExit();
        break;
    }

  }

  keyboard = SDL_GetKeyState(&size);

  for (j = 0; j < size; j++)
  {
    if (keyboard[j])
    {
      switch (j)
      {*/
/*        case SDLK_z:
          nearVal-=1;
          break;
        case SDLK_x:
          nearVal+=1;
          break;
        case SDLK_a:
          cameraZoom-=100;
          break;
        case SDLK_s:
          cameraZoom+=100;
          break;
        case SDLK_w:
          {
          float factor = (float)cameraY/(float)cameraZ;
          cameraY+=1;
          cameraZ=(float)cameraY/factor;
          break;
          }
        case SDLK_q:
          {
          float factor = (float)cameraY/(float)cameraZ;
          cameraY-=1;
          cameraZ=(float)cameraY/factor;
          break;
          }
        case SDLK_e:
          fov-=1;
          break;
        case SDLK_r:
          fov+=2;
          break; */
/*      case SDLK_RETURN:
          input2 = 0x1C;
          break;
      case SDLK_ESCAPE:
        input2 = 0x1B;
        break;

      case SDLK_UP:
          inputKey |= 1;
          break;

      case SDLK_DOWN:
          inputKey |= 2;
          break;

      case SDLK_RIGHT:
          inputKey |= 8;
          break;

      case SDLK_LEFT:
          inputKey |= 4;
          break;
      case SDLK_SPACE:
        input1 = 1;
        break;
#ifdef INTERNAL_DEBUGGER
      case SDLK_o:
        debufferVar_topCameraZoom+=100;
        break;
      case SDLK_p:
        debufferVar_topCameraZoom-=100;
        break;
      case SDLK_t:
        debuggerVar_topCamera = !debuggerVar_topCamera;

        if(!debuggerVar_topCamera)
        {
          backgroundMode = backgroundModeEnum_2D;
        }
        break;
      case SDLK_c:
        debuggerVar_noHardClip = !debuggerVar_noHardClip;
        break;
      case SDLK_d:
        debugger_enterMainDebug();
        break;
      case SDLK_b:
          backgroundMode = backgroundModeEnum_3D;
          break;
      case SDLK_n:
          backgroundMode = backgroundModeEnum_2D;
          break;
#endif
      }
    }
  }
#ifdef INTERNAL_DEBUGGER
  if(debuggerVar_topCamera)
  {
    backgroundMode = backgroundModeEnum_3D;
  }
#endif*/
}
