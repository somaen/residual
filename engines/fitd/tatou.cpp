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

// seg 4
#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/scummsys.h"
#include "common/system.h"
#include "common/events.h"
#include "common/textconsole.h"

#include "common.h"
#include "engines/fitd/gfx_base.h"
#include "engines/fitd/cos_table.h"
#include "engines/fitd/fitd.h"
#include "engines/fitd/input.h"


// TODO
using namespace Fitd;

//////////// stuff to move
void clearScreenTatou(void)
{
	for(int i=0;i<45120;i++)
	{
		screen[i] = 0;
	}
}

void blitScreenTatou(void)
{	
	for(int i=0;i<45120;i++)
	{
		unkScreenVar[i] = screen[i];
	}
}

void copyPalette(char* source, char* dest)
{
	for(int i=0;i<768;i++)
	{
		dest[i] = source[i];
	}
}

void copyToScreen(char* source, char* dest)
{
	for(int i=0;i<64000;i++)
	{
		dest[i] = source[i];
	}
}

void paletteFill(void* palette, unsigned char r, unsigned char g, unsigned b)
{
	warning("palettefill(,%d,%d,%d)",r,g,b);
	unsigned char* paletteLocal = (unsigned char*) palette;
	int offset = 0;
	
	r<<=1;
	g<<=1;
	b<<=1;
	
	for(int i=0;i<256;i++)
	{
		paletteLocal[offset] = r;
		paletteLocal[offset+1] = g;
		paletteLocal[offset+2] = b;
		offset += 3;
	}
}

void fadeInSub1(char* palette)
{	
	for(int i=0;i<256;i++)
	{
		  palette[i*3] >>=2;
		 palette[i*3+1] >>=2;
		 palette[i*3+2] >>=2; 
	}
}

void blitPalette(char* palettePtr,unsigned char startColor,unsigned char nbColor)
{

	char paletteRGBA[256*4];
	char* outPtr = scaledScreen;
	char* inPtr = unkScreenVar;
	
	Fitd::g_driver->getPalette(paletteRGBA);
	
	for(int i=startColor;i<startColor+nbColor;i++)
	{
		paletteRGBA[i*4] = palettePtr[i*3];
		paletteRGBA[i*4+1] = palettePtr[i*3+1];
		paletteRGBA[i*4+2] = palettePtr[i*3+2];
		paletteRGBA[i*4+3] = -1;
	}
	
	for(int i = 0; i < 200; i++)
	{
		char* copySource = outPtr;
		
		for(int j = 0; j < 320; j++)
		{
			*(outPtr++) = *(inPtr);
			*(outPtr++) = *(inPtr++);
		}
		
		// copy line
		for(int j = 0; j < 640; j++)
		{
			*(outPtr++) = *(copySource++);
		}
		
	}
	
	Fitd::g_driver->setPalette(paletteRGBA);
	Fitd::g_driver->flip((unsigned char*)scaledScreen);
}

void flipOtherPalette(char* palettePtr)
{
	int i;
	char paletteRGBA[256*4];
	char* outPtr = scaledScreen;
	char* inPtr = unkScreenVar;
	
	Fitd::g_driver->getPalette(paletteRGBA);
	
	for(i=0;i<256;i++)
	{
		paletteRGBA[i*4] = palettePtr[i*3];
		paletteRGBA[i*4+1] = palettePtr[i*3+1];
		paletteRGBA[i*4+2] = palettePtr[i*3+2];
		paletteRGBA[i*4+3] = -1;
	}
	
	for(i=0;i<200;i++)
	{
		int j;
		char* copySource = outPtr;
		
		for(j=0;j<320;j++)
		{
			*(outPtr++) = *(inPtr);
			*(outPtr++) = *(inPtr++);
		}
		
		// copy line
		for(j=0;j<640;j++)
		{
			*(outPtr++) = *(copySource++);
		}
		
	}
	
	Fitd::g_driver->setPalette(paletteRGBA);
	Fitd::g_driver->flip((unsigned char*)scaledScreen);
}

void computeProportionalPalette(unsigned char* inPalette, unsigned char* outPalette, int coef)
{
	int i;
	
	for(i=0;i<256;i++)
	{
		*(outPalette++) = ((*(inPalette++))*coef)>> 8;
		*(outPalette++) = ((*(inPalette++))*coef)>> 8;
		*(outPalette++) = ((*(inPalette++))*coef)>> 8;
	}
}

void make3dTatouUnk1(int var1,int var2)
{
	char localPalette[0x300];
	int i;
	
	freezeTime();
	
	if(paletteVar == 2) // only used for the ending ?
	{
	}
	else
	{
		for(i=0;i<256;i+=var1)
		{
			computeProportionalPalette((unsigned char*)Fitd::g_driver->_palette,(unsigned char*)localPalette,i);
			fadeInSub1(localPalette);
			flipOtherPalette(localPalette);
		}
	}
	
	paletteVar = 1;
	
	flip();
	
	unfreezeTime();
}

void fadeOut(int var1, int var2)
{
	char localPalette[0x300];
	
	freezeTime();
	
	for(int i=256;i>=0;i-=var1)
	{
		computeProportionalPalette((unsigned char*)Fitd::g_driver->_palette,(unsigned char*)localPalette,i);
		fadeInSub1(localPalette);
		flipOtherPalette(localPalette);
	}
	
	unfreezeTime();
}

void fadeIn(void* sourcePal)
{
	char localPalette[0x300];
	
	copyPalette((char*)sourcePal,localPalette);
	
	fadeInSub1(localPalette);
	
	flipOtherPalette(localPalette);
}

void flip()
{
	char* outPtr = scaledScreen;
	char* inPtr = unkScreenVar;

	char paletteRGBA[256*4];
	
#ifdef USE_GL
	Fitd::g_driver->flip(NULL);
	return;
#endif
	
	for(int i = 0; i < 256; i++)
	{
		paletteRGBA[i*4] = Fitd::g_driver->_palette[i*3];
		paletteRGBA[i*4+1] = Fitd::g_driver->_palette[i*3+1];
		paletteRGBA[i*4+2] = Fitd::g_driver->_palette[i*3+2];
		paletteRGBA[i*4+3] = -1;
	}
	
	for(int i = 0; i < 200; i++)
	{
		char* copySource = outPtr;
		
		for(int j = 0; j < 320; j++)
		{
			*(outPtr++) = *(inPtr);
			*(outPtr++) = *(inPtr++);
		}
		
		// copy line
		for(int j = 0; j < 640; j++)
		{
			*(outPtr++) = *(copySource++);
		}
		
	}
	
	Fitd::g_driver->setPalette(paletteRGBA);
	Fitd::g_driver->flip((unsigned char*)scaledScreen);
}

void process_events()
{
	Common::Event handledEvent;
	while (g_system->getEventManager()->pollEvent(handledEvent)) {
		// Handle any buttons, keys and joystick operations
		Common::EventType type = handledEvent.type;
		if (type == Common::EVENT_KEYDOWN) {
				warning("Key %d", handledEvent.kbd.keycode);
				handleKey(type, handledEvent.kbd.keycode, handledEvent.kbd.flags, handledEvent.kbd.ascii);
				//}
		}
		//if (type == Common::EVENT_KEYDOWN || type == Common::EVENT_KEYUP) {
			//handleKey(type, event.kbd.keycode, event.kbd.flags, event.kbd.ascii);
			//}
		// Check for "Hard" quit"
		if (type == Common::EVENT_QUIT) {
			cleanupAndExit();
			error("Hard quit");
			return;
		}
		if (type == Common::EVENT_SCREEN_CHANGED)
			return;
			//_refreshDrawNeeded = true;
	}
}

void startChrono(unsigned int* chrono)
{
	*chrono = g_fitd->getFrameNum();
}

int evalChrono(unsigned int* chrono)
{
	return(g_fitd->getFrameNum()-*chrono);
}

// bp = x, bx = y, cx = z
// out
// xOut = dx, yOut = ax
void makeRotationMtx(unsigned int x, unsigned int y, unsigned int z, int* xOut, int* yOut)
{
	if(x)
	{
		int var1 = (((cosTable[(x+0x100)&0x3FF] * y) << 1) &0xFFFF0000) -  (((cosTable[x&0x3FF] *z) <<1) & 0xFFFF0000);
		int var2 = (((cosTable[x&0x3FF] * y) << 1) & 0xFFFF0000) + (((cosTable[(x+0x100)&0x3FF] *z) <<1) & 0xFFFF0000);
		
		*yOut = var1>>16;
		*xOut = var2>>16;
	}
	else
	{
		*xOut = z;
		*yOut = y;
	}
}

void rotateModel(int x,int y,int z,int alpha,int beta,int gamma,int time)
{
	int x1, y1, x2, y2;
	
	makeRotationMtx(alpha+0x200,-time,0,&x1,&y1);
	makeRotationMtx(beta+0x200,y1,0,&x2,&y2);
	
	setupSelfModifyingCode(x2+x,-x1+y,y2+z);
	setupPointTransformSM(alpha,beta,gamma);
}

void playSound(int num)
{
	int size = getPakSize("listsamp",num);
	char* ptr = HQR_Get(listSamp,num);
	
#ifndef NO_SOUND
	// TODO fixsound
	// Fitd::g_driver->playSample(ptr,size);
#endif
}

////////////////////////

int make3dTatou(void)
{
	char* tatou2d;
	char* tatou3d;
	char* tatouPal;
	int time;
	int deltaTime;
	int rotation;
	int unk1;
	char paletteBackup[768];
	unsigned int localChrono;
	
	tatou2d = loadPakSafe("ITD_RESS",2);
	tatou3d = loadPakSafe("ITD_RESS",0);
	tatouPal = loadPakSafe("ITD_RESS",1);
	
	time = 8920;
	deltaTime = 50;
	rotation = 256;
	unk1 = 8;
	
	setupSMCode(160,100,128,500,490);
	
	copyPalette(Fitd::g_driver->_palette,paletteBackup);
	
	paletteFill(Fitd::g_driver->_palette,0,0,0);
	
	fadeIn(Fitd::g_driver->_palette);
	
	copyPalette(tatouPal,Fitd::g_driver->_palette);
	copyToScreen(tatou2d+770,unkScreenVar);
	copyToScreen(unkScreenVar,aux2);
	
#ifdef USE_GL
	Fitd::g_driver->CopyBlockPhys((unsigned char*)unkScreenVar,0,0,320,200);
	flip();
#endif
	
	make3dTatouUnk1(8,0);
	
	startChrono(&localChrono);
	
	do
	{
		process_events();
		readKeyboard();
		
		if(evalChrono(&localChrono)<=180) // avant eclair
		{
#ifdef USE_GL
			Fitd::g_driver->startFrame();
#endif
			
			if(input2 || input1)
			{
				break;
			}
			
#ifdef USE_GL
			Fitd::g_driver->stopFrame();
			flip();
#endif
		}
		else // eclair
		{
			/*  soundVar2 = -1;
			 soundVar1 = -1; */
			
			playSound(CVars[getCVarsIdx(SAMPLE_TONNERRE)]);
			
			/*     soundVar2 = -1;
			 soundVar1 = -1;*/
			
			paletteFill(Fitd::g_driver->_palette,63,63,63);
			fadeIn(Fitd::g_driver->_palette);
			/*  setClipSize(0,0,319,199);*/
			
			clearScreenTatou();
			
			rotateModel(0,0,0,unk1,rotation,0,time);
			
			renderModel(0,0,0,0,0,0,tatou3d);
			
			blitScreenTatou();
#ifdef USE_GL
			Fitd::g_driver->CopyBlockPhys((unsigned char*)unkScreenVar,0,0,320,200);
#endif
			
			copyPalette(tatouPal,Fitd::g_driver->_palette);
			fadeIn(Fitd::g_driver->_palette);
			
			while(input2==0 && input1 == 0 && inputKey == 0) // boucle de rotation du tatou
			{
				process_events();
				
				time+=deltaTime-25;
				
				if(time>16000)
					break;
				
				rotation -=8;
				
				clearScreenTatou();
				
#ifdef USE_GL
				Fitd::g_driver->startFrame();
#endif
				
				rotateModel(0,0,0,unk1,rotation,0,time);
				
				renderModel(0,0,0,0,0,0,tatou3d);
				
				blitScreenTatou();
				
#ifdef USE_GL
				Fitd::g_driver->stopFrame();
#endif
				
				flip();
			}
			
			break;
		}
	}while(1);
	
	free(tatouPal);
	free(tatou3d);
	free(tatou2d);
	
	if(input2 || input1 || inputKey)
	{
		while(input2)
		{
			process_events();
			//readKeyboard();
		}
		
		fadeOut(32,0);
		copyPalette(paletteBackup,Fitd::g_driver->_palette);
		return(1);
	}
	else
	{
		fadeOut(16,0);
		copyPalette(paletteBackup,Fitd::g_driver->_palette);
		return(0);
	}
	
	return(0);
}
