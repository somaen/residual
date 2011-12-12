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

#ifndef GRIM_GFX_OPENGL_H
#define GRIM_GFX_OPENGL_H

#include "engines/fitd/gfx_base.h"

//#ifdef USE_OPENGL

#if defined (SDL_BACKEND) && !defined(__amigaos4__)
#include <SDL_opengl.h>
#undef ARRAYSIZE
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "common/textconsole.h"

#define USE_GL

namespace Fitd {

class ModelNode;
class Mesh;
class MeshFace;

class GfxOpenGL : public GfxBase {
public:
	GfxOpenGL();
	virtual ~GfxOpenGL();

	byte *setupScreen(int screenW, int screenH, bool fullscreen);
	void initExtensions();

	const char *getVideoDeviceName();

	void setupCamera(float fov, float nclip, float fclip, float roll);
	void positionCamera(Math::Vector3d pos, Math::Vector3d interest);

	void clearScreen();
	void flipBuffer();

	bool isHardwareAccelerated() { return true; } // CLEANME

	void initBuffer(char *buffer, int width, int height);
	void initVideoBuffer(char *buffer, int width, int height) {warning("initVideoBuffer");} // TODO 
	void putpixel(int x, int y, int pixel) {warning("putpixel");} // TODO
	void setColor(byte i, byte R, byte G, byte B) {warning("setColor");} // TODO
	void setPalette(byte * palette);
	void setPalette320x200(byte * palette) {warning("setPalette320x200");} // TODO
	void flip(unsigned char *videoBuffer);
	void draw320x200BufferToScreen(unsigned char *videoBuffer) {warning("draw320x200BufferToScreen");} // TODO
	void CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom);
	void drawText(int X, int Y, char *text) {warning("Unimplemented: drawText");} // TODO
	void drawTextColor(int X, int Y, char *string, unsigned char R, unsigned char G, unsigned char B) {warning("drawTextColor");} // TODO
	void drawLine(int X1,int X2,int Y1,int Y2,unsigned char color, unsigned char* palette) {warning("drawLine");} // TODO
	void getPalette(char* palette);
	void set320x200Mode(bool mode) {warning("set320x200mode");} // TODO
	
	void startFrame();
	void stopFrame() {} // TODO
	void startModelRender();
	void stopModelRender();
//#ifdef USE_GL
	void fillPoly(float* buffer, int numPoint, unsigned char color,uint8 polyType);
	void draw3dLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color);
	void draw3dQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, unsigned char color, int transparency);
	void cleanScreenKeepZBuffer();
	void drawSphere(float X, float Y, float Z, uint8 color, float size);
	
	void startBgPoly();
	void endBgPoly();
	void addBgPolyPoint(int x, int y);
//#else
#if 0
	void fillPoly(short int* buffer, int numPoint, unsigned char color);
	void draw3dLine(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char color);
	void draw3dQuad(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int x4, int y4, int z4, unsigned char color);
#endif
	
#ifdef INTERNAL_DEBUGGER
	void drawDebugText(const u32 X, const u32 Y, const uint8* string);
#endif
	
	void fadeBlackToWhite() {warning("fadeBlackToWhite");} // TODO
	void updateImage() {warning("updateImage");} // TODO
	
	// only here:
	void init();
private:
	GLuint _emergFont;
	int _smushNumTex;
	GLuint *_smushTexIds;
	int _smushWidth;
	int _smushHeight;
	byte *_storedDisplay;
	bool _useDepthShader;
	GLuint _fragmentProgram;
};

} // end of namespace Grim

//#endif

#endif
