/* Residual - A 3D game interpreter
 *
 * Residual is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 */

#ifndef GRIM_GFX_BASE_H
#define GRIM_GFX_BASE_H

#define USE_GL

#include "math/vector3d.h"

namespace Graphics {
	struct Surface;
}

namespace Fitd {

class GfxBase {
public:
	GfxBase();
	virtual ~GfxBase() { ; }

	/**
	 * Creates a render-context.
	 *
	 * @param screenW		the width of the context
	 * @param screenH		the height of the context
	 * @param fullscreen	true if fullscreen is desired, false otherwise.
	 */
	virtual byte *setupScreen(int screenW, int screenH, bool fullscreen) = 0;

	/**
	 *	Query whether the current context is hardware-accelerated
	 *
	 * @return true if hw-accelerated, false otherwise
	 */
	virtual bool isHardwareAccelerated() = 0;
	virtual void initBuffer(char *buffer, int width, int height) = 0;
	virtual	void initVideoBuffer(char *buffer, int width, int height) = 0;
	virtual void putpixel(int x, int y, int pixel) = 0;
	virtual void setColor(byte i, byte R, byte G, byte B) = 0;
	virtual void setPalette(byte * palette) = 0;
	virtual void setPalette320x200(byte * palette) = 0;
	virtual void flip(unsigned char *videoBuffer) = 0;
	virtual void draw320x200BufferToScreen(unsigned char *videoBuffer) = 0;
	virtual void CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom) = 0;
	virtual void drawText(int X, int Y, char *text) = 0;
	virtual void drawTextColor(int X, int Y, char *string, unsigned char R, unsigned char G, unsigned char B) = 0;
	virtual void drawLine(int X1,int X2,int Y1,int Y2,unsigned char color, unsigned char* palette) = 0;
	virtual void getPalette(char* palette) = 0;
	virtual void set320x200Mode(bool mode) = 0;
	
	virtual void startFrame() = 0;
	virtual void stopFrame() = 0;
	virtual void startModelRender() = 0;
	virtual void stopModelRender() = 0;
#ifdef USE_GL
	virtual void fillPoly(float* buffer, int numPoint, unsigned char color,uint8 polyType) = 0;
	virtual void draw3dLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color) = 0;
	virtual void draw3dQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, unsigned char color, int transparency) = 0;
	virtual void cleanScreenKeepZBuffer() = 0;
	virtual void drawSphere(float X, float Y, float Z, uint8 color, float size) = 0;
	
	virtual void startBgPoly() = 0;
	virtual void endBgPoly() = 0;
	virtual void addBgPolyPoint(int x, int y) = 0;
#else
	virtual void fillPoly(short int* buffer, int numPoint, unsigned char color) = 0;
	virtual void draw3dLine(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char color) = 0;
	virtual void draw3dQuad(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int x4, int y4, int z4, unsigned char color) = 0;
#endif
	
#ifdef INTERNAL_DEBUGGER
	virtual void drawDebugText(const u32 X, const u32 Y, const uint8* string) = 0;
#endif

	virtual void fadeBlackToWhite() = 0;
	virtual void updateImage() = 0;
	
	char *_palette;
	void clearPalette();
protected:
	int _screenWidth, _screenHeight, _screenBPP;
	bool _isFullscreen;
	//Shadow *_currentShadowArray;
	unsigned char _shadowColorR;
	unsigned char _shadowColorG;
	unsigned char _shadowColorB;
	bool _renderBitmaps;
	bool _renderZBitmaps;
};

// Factory-like functions:

GfxBase *CreateGfxOpenGL();
GfxBase *CreateGfxTinyGL();

extern GfxBase *g_driver;

} // end of namespace Fitd

#endif
