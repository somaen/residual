/* Residual - A 3D game interpreter
 *
 * Residual is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
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

#ifndef GRIM_GFX_TINYGL_H
#define GRIM_GFX_TINYGL_H

#include "engines/fitd/gfx_base.h"

#include "graphics/tinygl/zgl.h"

namespace Fitd {

class ModelNode;
class Mesh;
class MeshFace;

class GfxTinyGL : public GfxBase {
public:
	GfxTinyGL();
	virtual ~GfxTinyGL();

	byte *setupScreen(int screenW, int screenH, bool fullscreen);

	const char *getVideoDeviceName();

	void setupCamera(float fov, float nclip, float fclip, float roll);
	void positionCamera(Math::Vector3d pos, Math::Vector3d interest);

	void clearScreen();
	void flipBuffer();

	bool isHardwareAccelerated();

	//	void storeDisplay();
	//void copyStoredToDisplay();
	void initBuffer(char *buffer, int width, int height);
	void initVideoBuffer(char *buffer, int width, int height) {} // TODO 
	void putpixel(int x, int y, int pixel) {} // TODO
	void setColor(byte i, byte R, byte G, byte B) {} // TODO
	void setPalette(byte * palette);
	void setPalette320x200(byte * palette) {} // TODO
	void flip(unsigned char *videoBuffer);
	void draw320x200BufferToScreen(unsigned char *videoBuffer) {} // TODO
	void CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom);
	void drawText(int X, int Y, char *text) {} // TODO
	void drawTextColor(int X, int Y, char *string, unsigned char R, unsigned char G, unsigned char B) {} // TODO
	void drawLine(int X1,int X2,int Y1,int Y2,unsigned char color, unsigned char* palette) {} // TODO
	void getPalette(char* palette);
	void set320x200Mode(bool mode) {} // TODO
	
	void startFrame();
	void stopFrame() {} // TODO
	void startModelRender() {}// TODO
	void stopModelRender() {} // TODO
							  //#ifdef USE_GL
	void fillPoly(float* buffer, int numPoint, unsigned char color,uint8 polyType);
	void draw3dLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color);
	void draw3dQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, unsigned char color, int transparency);
	void cleanScreenKeepZBuffer();
	void drawSphere(float X, float Y, float Z, uint8 color, float size);
	
	void startBgPoly() {} // TODO
	void endBgPoly() {} // TODO
	void addBgPolyPoint(int x, int y) {} // TODO
										 //#else
#if 0
	void fillPoly(short int* buffer, int numPoint, unsigned char color);
	void draw3dLine(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char color);
	void draw3dQuad(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int x4, int y4, int z4, unsigned char color);
#endif
	
#ifdef INTERNAL_DEBUGGER
	void drawDebugText(const u32 X, const u32 Y, const uint8* string);
#endif
	
	void fadeBlackToWhite() {} // TODO
	void updateImage() {} // TODO

protected:

private:
	TinyGL::ZBuffer *_zb;
	byte *_screen;
	byte *_smushBitmap;
	int _smushWidth;
	int _smushHeight;
	byte *_storedDisplay;
};

} // end of namespace Grim

#endif
