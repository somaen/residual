/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "engines/wintermute/base/base_file_manager.h"
#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/gfx/opengl/base_surface_opengl.h"
#include "engines/wintermute/base/gfx/opengl/base_render_opengl.h"
#include "engines/wintermute/base/gfx/opengl/texture.h"
#include "engines/wintermute/base/gfx/base_image.h"
#include "engines/wintermute/platform_osystem.h"
#include "graphics/transparent_surface.h"
#include "graphics/transform_tools.h"
#include "graphics/pixelformat.h"
#include "graphics/surface.h"
#include "common/stream.h"
#include "common/system.h"

namespace Wintermute {

//////////////////////////////////////////////////////////////////////////
BaseSurfaceOpenGL::BaseSurfaceOpenGL(BaseGame *inGame) : BaseSurface(inGame) {
	_surface = new Graphics::Surface();
	_alphaMask = nullptr;
	_alphaType = Graphics::ALPHA_FULL;
	_lockPixels = nullptr;
	_lockPitch = 0;
	_loaded = false;
	_texture.reset();
}

//////////////////////////////////////////////////////////////////////////
BaseSurfaceOpenGL::~BaseSurfaceOpenGL() {
	if (_surface) {
		_surface->free();
		delete _surface;
		_surface = nullptr;
	}

	delete[] _alphaMask;
	_alphaMask = nullptr;

	_gameRef->addMem(-_width * _height * 4);
}

static Graphics::AlphaType hasTransparencyType(const Graphics::Surface *surf) {
	if (surf->format.bytesPerPixel != 4) {
		warning("hasTransparencyType:: non 32 bpp surface passed as argument");
		return Graphics::ALPHA_OPAQUE;
	}
	uint8 r, g, b, a;
	bool seenAlpha = false;
	bool seenFullAlpha = false;
	for (int i = 0; i < surf->h; i++) {
		if (seenFullAlpha) {
			break;
		}
		for (int j = 0; j < surf->w; j++) {
			uint32 pix = *(const uint32 *)surf->getBasePtr(j, i);
			surf->format.colorToARGB(pix, a, r, g, b);
			if (a != 255) {
				seenAlpha = true;
				if (a != 0) {
					seenFullAlpha = true;
					break;
				}
			}
		}
	}
	if (seenFullAlpha) {
		return Graphics::ALPHA_FULL;
	} else if (seenAlpha) {
		return Graphics::ALPHA_BINARY;
	} else {
		return Graphics::ALPHA_OPAQUE;
	}
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::create(const Common::String &filename, bool defaultCK, byte ckRed, byte ckGreen, byte ckBlue, int lifeTime, bool keepLoaded) {
	/*  BaseRenderOpenGL *renderer = static_cast<BaseRenderOpenGL *>(_gameRef->_renderer); */
	_filename = filename;
//	const Graphics::Surface *surface = image->getSurface();

	if (defaultCK) {
		ckRed   = 255;
		ckGreen = 0;
		ckBlue  = 255;
	}

	_ckDefault = defaultCK;
	_ckRed = ckRed;
	_ckGreen = ckGreen;
	_ckBlue = ckBlue;

	if (_lifeTime == 0 || lifeTime == -1 || lifeTime > _lifeTime) {
		_lifeTime = lifeTime;
	}

	_keepLoaded = keepLoaded;
	if (_keepLoaded) {
		_lifeTime = -1;
	}

	return STATUS_OK;
}

bool BaseSurfaceOpenGL::finishLoad() {
	BaseImage *image = new BaseImage();
	if (!image->loadFile(_filename)) {
		delete image;
		return false;
	}

	_width = image->getSurface()->w;
	_height = image->getSurface()->h;

	bool isSaveGameGrayscale = _filename.matchString("savegame:*g", true);
	if (isSaveGameGrayscale) {
		warning("grayscaleConversion not yet implemented");
		// FIBITMAP *newImg = FreeImage_ConvertToGreyscale(img); TODO
	}

	_surface->free();
	delete _surface;

	bool needsColorKey = false;
	bool replaceAlpha = true;
	if (image->getSurface()->format.bytesPerPixel == 1) {
		if (!image->getPalette()) {
			error("Missing palette while loading 8bit image %s", _filename.c_str());
		}
		_surface = image->getSurface()->convertTo(_gameRef->_renderer->getScreenFormat(), image->getPalette());
		needsColorKey = true;
	} else {
		if (image->getSurface()->format != _gameRef->_renderer->getScreenFormat()) {
			_surface = image->getSurface()->convertTo(_gameRef->_renderer->getScreenFormat());
		} else {
			_surface = new Graphics::Surface();
			_surface->copyFrom(*image->getSurface());
		}

		if (_filename.hasSuffix(".bmp") && image->getSurface()->format.bytesPerPixel == 4) {
			// 32 bpp BMPs have nothing useful in their alpha-channel -> color-key
			needsColorKey = true;
			replaceAlpha = false;
		} else if (image->getSurface()->format.aBits() == 0) {
			needsColorKey = true;
		}
	}

	if (needsColorKey) {
		Graphics::TransparentSurface trans(*_surface);
		trans.applyColorKey(_ckRed, _ckGreen, _ckBlue, replaceAlpha);
	}

	_alphaType = hasTransparencyType(_surface);
	_valid = true;

	_gameRef->addMem(_width * _height * 4);

	delete image;

	_texture = Common::SharedPtr<Texture>(new Texture(*_surface));

	_loaded = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void BaseSurfaceOpenGL::genAlphaMask(Graphics::Surface *surface) {
	warning("BaseSurfaceOpenGL::GenAlphaMask - Not ported yet");
	return;
	// TODO: Reimplement this
	delete[] _alphaMask;
	_alphaMask = nullptr;
	if (!surface) {
		return;
	}

	bool hasColorKey;
	/* uint32 colorKey; */
	uint8 ckRed, ckGreen, ckBlue;
	/*  if (SDL_GetColorKey(surface, &colorKey) == 0) {
	        hasColorKey = true;
	        SDL_GetRGB(colorKey, surface->format, &ckRed, &ckGreen, &ckBlue);
	    } else hasColorKey = false;
	*/
	_alphaMask = new byte[surface->w * surface->h];

	bool hasTransparency = false;
	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			uint32 pixel = getPixelAt(surface, x, y);

			uint8 r, g, b, a;
			surface->format.colorToARGB(pixel, a, r, g, b);
			//SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

			if (hasColorKey && r == ckRed && g == ckGreen && b == ckBlue) {
				a = 0;
			}

			_alphaMask[y * surface->w + x] = a;
			if (a < 255) {
				hasTransparency = true;
			}
		}
	}

	if (!hasTransparency) {
		delete[] _alphaMask;
		_alphaMask = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
uint32 BaseSurfaceOpenGL::getPixelAt(Graphics::Surface *surface, int x, int y) {
	warning("BaseSurfaceOpenGL::GetPixel - Not ported yet");
	int bpp = surface->format.bytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	uint8 *p = (uint8 *)surface->getBasePtr(x, y);

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(uint16 *)p;
		break;

	case 3:
#ifdef SCUMM_BIG_ENDIAN
		//  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		return p[0] << 16 | p[1] << 8 | p[2];
#else
		//else
		return p[0] | p[1] << 8 | p[2] << 16;
#endif
		break;

	case 4:
		return *(uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::create(int width, int height) {
	_width = width;
	_height = height;

	_gameRef->addMem(_width * _height * 4);

	_valid = true;

	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::isTransparentAt(int x, int y) {
	return isTransparentAtLite(x, y);
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::isTransparentAtLite(int x, int y) {
	if (x < 0 || x >= _surface->w || y < 0 || y >= _surface->h) {
		return true;
	}

	if (_surface->format.bytesPerPixel == 4) {
		uint32 pixel = *(uint32 *)_surface->getBasePtr(x, y);
		uint8 r, g, b, a;
		_surface->format.colorToARGB(pixel, a, r, g, b);
		if (a <= 128) {
			return true;
		} else {
			return false;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::startPixelOp() {
	//SDL_LockTexture(_texture, nullptr, &_lockPixels, &_lockPitch);
	// Any pixel-op makes the caching useless:

	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::endPixelOp() {
	//SDL_UnlockTexture(_texture);
	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceOpenGL::drawSprite(int x, int y, Rect32 *rect, Rect32 *newRect, Graphics::TransformStruct transform) {
	BaseRenderOpenGL *renderer = static_cast<BaseRenderOpenGL *>(_gameRef->_renderer);

	if (!_loaded) {
		finishLoad();
	}

	if (renderer->_forceAlphaColor != 0) {
		transform._rgbaMod = renderer->_forceAlphaColor;
	}

	// TODO: This _might_ miss the intended behaviour by 1 in each direction
	// But I think it fits the model used in Wintermute.
	Common::Rect srcRect;
	srcRect.left = rect->left;
	srcRect.top = rect->top;
	srcRect.setWidth(rect->right - rect->left);
	srcRect.setHeight(rect->bottom - rect->top);

	Common::Rect position;

	if (newRect) {
		position.top = y;
		position.left = x;
		position.setWidth(newRect->width());
		position.setHeight(newRect->height());
	} else {

		Common::Rect r;
		r.top = 0;
		r.left = 0;
		r.setWidth(rect->width());
		r.setHeight(rect->height());

		r = Graphics::TransformTools::newRect(r, transform, 0);

		position.top = r.top + y + transform._offset.y;
		position.left = r.left + x + transform._offset.x;
		position.setWidth(r.width() * transform._numTimesX);
		position.setHeight(r.height() * transform._numTimesY);
	}
	renderer->modTargetRect(&position);

	// TODO: This actually requires us to have the SAME source-offsets every time,
	// But no checking is in place for that yet.

	// Optimize by not doing alpha-blits if we lack alpha
	if (_alphaType == Graphics::ALPHA_OPAQUE && !transform._alphaDisable) {
		transform._alphaDisable = true;
	}
//	warning("Draw %s %d %d %d %d  %d %d  %d %d", _filename.c_str(), position.left, position.right, position.bottom, position.top, position.width(), position.height(), _width, _height);

//	renderer->drawSurface(this, _surface, &srcRect, &position, transform);
	renderer->drawTexture(this, *_texture, &srcRect, &position, transform);
	return STATUS_OK;
}

bool BaseSurfaceOpenGL::putSurface(const Graphics::Surface &surface, bool hasAlpha) {
	_loaded = true;

	if (surface.format == _surface->format && surface.pitch == _surface->pitch && surface.h == _surface->h) {
		const byte *src = (const byte *)surface.getBasePtr(0, 0);
		byte *dst = (byte *)_surface->getBasePtr(0, 0);
		memcpy(dst, src, surface.pitch * surface.h);
	} else {
		_surface->free();
		_surface->copyFrom(surface);
	}
	if (hasAlpha) {
		_alphaType = Graphics::ALPHA_FULL;
	} else {
		_alphaType = Graphics::ALPHA_OPAQUE;
	}

	_texture = Common::SharedPtr<Texture>(new Texture(*_surface));

	return STATUS_OK;
}

} // End of namespace Wintermute
