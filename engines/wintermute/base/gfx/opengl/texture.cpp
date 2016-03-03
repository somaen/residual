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
#include "common/textconsole.h"
#include "engines/wintermute/base/gfx/opengl/texture.h"

namespace Wintermute {

Texture::TexId::TexId() {
	glGenTextures(1, &_texId);
}

Texture::TexId::~TexId() {
	glDeleteTextures(1, &_texId);
}

Texture::Texture(const Graphics::Surface &surface) {
	init(surface);
}

Texture::Texture(const Texture &other) {
	copyFrom(other);
}

Texture &Texture::operator=(const Texture &rhs) {
	cleanup();
	copyFrom(rhs);
	return *this;
}

void Texture::init(const Graphics::Surface &surface) {
	_width = surface.w;
	_height = surface.h;
	_texIdPtr = Common::SharedPtr<TexId>(new TexId());

	Graphics::PixelFormat sourceFormat = surface.format;

	GLint internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;
	GLenum dataType = GL_UNSIGNED_BYTE;
	Graphics::PixelFormat pixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
	_hasAlpha = true;
	const void *dataPtr = surface.getPixels();
	Graphics::Surface *convertedSurface = nullptr;
	//                                                      R  G  B    A
//	Graphics::PixelFormat pixelFormat(4, 8, 8, 8, 8, 8, 16, 24, 0);
	//                                                      A R  G  B
	if (sourceFormat == Graphics::PixelFormat(4, 8, 8, 8, 0, 8, 16, 24, 0)) {
		internalFormat = GL_RGB;
		format = GL_BGRA;
		dataType = GL_UNSIGNED_INT_8_8_8_8;
		// Block conversion
		pixelFormat = sourceFormat;
	} else {
		convertedSurface = surface.convertTo(pixelFormat, /*image->getPalette()*/nullptr);
		dataPtr = convertedSurface->getPixels();
	}

	glBindTexture(GL_TEXTURE_2D, getTexId());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format, dataType, dataPtr);
	
	if (convertedSurface) {
		convertedSurface->free();
		delete convertedSurface;
	}
}

uint32 Texture::getTexId() const {
	return _texIdPtr->_texId;
}

void Texture::cleanup() {
	_texIdPtr.reset();
}

void Texture::copyFrom(const Texture &other) {
	_texIdPtr = other._texIdPtr;
	_width = other._width;
	_height = other._height;
	_hasAlpha = other._hasAlpha;
}

Texture::~Texture() {
	cleanup();
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, getTexId());
}

} // End of namespace Wintermute

#endif