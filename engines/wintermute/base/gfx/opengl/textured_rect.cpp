/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
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
 *
 */

#include "common/scummsys.h"

#ifdef USE_OPENGL

#ifdef USE_GLEW
#include <GL/glew.h>
#elif defined (SDL_BACKEND) && !defined(__amigaos4__)
#include <SDL_opengl.h>
#undef ARRAYSIZE
#else
#include <GL/gl.h>
#endif

#include "common/rect.h"
#include "common/textconsole.h"
#include "engines/wintermute/base/gfx/opengl/texture.h"
#include "engines/wintermute/base/gfx/opengl/textured_rect.h"

namespace Wintermute {

TexturedRect::TexturedRect(const Texture &texture) : _texture(texture) {

}

void TexturedRect::draw(Common::Rect srcRect, Common::Rect dstRect, const Graphics::TransformStruct &transform) {
	dstRect.left += transform._offset.x;
	dstRect.right += transform._offset.x;
	dstRect.top += transform._offset.y;
	dstRect.bottom += transform._offset.y;

	uint32 rMask = (transform._rgbaMod & 0xFF000000) >> 24;
	uint32 gMask = (transform._rgbaMod & 0x00FF0000) >> 16;
	uint32 bMask = (transform._rgbaMod & 0x0000FF00) >> 8;
	uint32 aMask = (transform._rgbaMod & 0x000000FF) >> 0;

	float maxTexX = srcRect.right / (float) _texture.getWidth();
	float minTexX = srcRect.left / (float) _texture.getWidth();

	float minTexY = srcRect.top / (float) _texture.getHeight();
	float maxTexY = srcRect.bottom / (float) _texture.getHeight();
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	// draw
	glDisable(GL_DEPTH_TEST);
	if (transform._alphaDisable || _texture.hasAlpha() == false) {
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	} else {
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
//	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_TEXTURE);
	_texture.bind();

	glBegin(GL_QUADS);
	glColor4f(rMask / 255.0f, gMask / 255.0f, bMask / 255.0f, aMask / 255.0f);
	glTexCoord2f(minTexX, maxTexY);
	glVertex2f(dstRect.left, dstRect.bottom);
	glTexCoord2f(maxTexX, maxTexY);
	glVertex2f(dstRect.right, dstRect.bottom);
	glTexCoord2f(maxTexX, minTexY);
	glVertex2f(dstRect.right, dstRect.top);
	glTexCoord2f(minTexX, minTexY);
	glVertex2f(dstRect.left, dstRect.top);
	glEnd();
}

void TexturedRect::draw(int x, int y) const {
	Common::Rect position;
	position.setHeight(600);
	position.setWidth(800);

	Common::Rect dstRect;
	dstRect.left = x;
	dstRect.top = y;
	dstRect.setWidth(_texture.getWidth());
	dstRect.setHeight(_texture.getHeight());
}
#if 0
static void drawRectAndFlip(Graphics::Surface *surface) {
	float minTexX = 0.0f;
	float minTexY = 0.0f;
	float maxTexX = 1.0f;
	float maxTexY = 1.0f;

	Common::Rect position;
	position.setHeight(600);
	position.setWidth(800);

	GLuint _texId;
	int _width = surface->w;
	int _height = surface->h;

	glGenTextures(1, &_texId);

	glBindTexture(GL_TEXTURE_2D, _texId);

	Graphics::PixelFormat pixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
	Graphics::Surface *convertedSurface = surface->convertTo(pixelFormat, /*image->getPalette()*/nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->getPixels());
	convertedSurface->free();
	delete convertedSurface;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	// draw
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
//	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, _texId);

	glBegin(GL_QUADS);
//		glColor3f(1.0f * x, 1.0f * y, 0.5f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5);
	glTexCoord2f(minTexX, maxTexY);
	glVertex2f(position.left, position.bottom);
	glTexCoord2f(maxTexX, maxTexY);
	glVertex2f(position.right, position.bottom);
	glTexCoord2f(maxTexX, minTexY);
	glVertex2f(position.right, position.top);
	glTexCoord2f(minTexX, minTexY);
	glVertex2f(position.left, position.top);
	glEnd();

//		g_system->updateScreen();

	glDeleteTextures(1, &_texId);
}
#endif
} // End of namespace Wintermute

#endif