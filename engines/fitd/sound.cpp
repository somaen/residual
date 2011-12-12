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

#include "common/scummsys.h"
#include "engines/fitd/sound.h"

void Sound_Quit(void)
{/*
	Mix_HookMusic(NULL, NULL);
	Mix_CloseAudio();*/
}

void osystem_crossFade(char *buffer, char *palette)
{
}

int posInStream = 0;
volatile bool deviceStatus = false;

void my_audio_callback(void *userdata, uint8 *stream, int len)
{
	/*  Sound_Sample *sample = (Sound_Sample *)userdata;
	 Uint8* input = (Uint8*)sample->buffer;
	 
	 if((unsigned int)posInStream+len < sample->buffer_size)
	 {
	 memcpy(stream,input+posInStream,len);
	 posInStream+=len;
	 }
	 else
	 {
	 len = sample->buffer_size - posInStream;
	 memcpy(stream,input+posInStream,len);
	 posInStream+=len;
	 }*/
}

#ifdef USE_UNPACKED_DATA
void osystem_playSampleFromName(char* sampleName)
{
/*	
	Mix_Chunk *sample;
	
	sample=Mix_LoadWAV_RW(SDL_RWFromFile(sampleName, "rb"), 1);
	
	if(!sample)
	{
#ifdef INTERNAL_DEBUGGER
		printf("Mix_LoadWAV_RW: %s\n", Mix_GetError());
#endif
	}
	else
	{
		Mix_PlayChannel(-1, sample, 0);
	}*/
}
#else
void osystem_playSample(char* samplePtr,int size)
{/*
	
	Mix_Chunk *sample;
	
	sample=Mix_LoadWAV_RW(SDL_RWFromConstMem(samplePtr, size), 1);
	
	if(!sample)
	{
#ifdef INTERNAL_DEBUGGER
		printf("Mix_LoadWAV_RW: %s\n", Mix_GetError());
#endif
	}
	else
	{
		Mix_PlayChannel(-1, sample, 0);
	}*/
}
#endif
