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

#ifndef WINTERMUTE_BASE_SURFACE_OPENGL_H
#define WINTERMUTE_BASE_SURFACE_OPENGL_H

#include "graphics/surface.h"
#include "graphics/transparent_surface.h"
#include "engines/wintermute/base/gfx/base_surface.h"
#include "common/list.h"
#include "common/ptr.h"

namespace Wintermute {
struct TransparentSurface;
class BaseImage;
class Texture;
class BaseSurfaceOpenGL : public BaseSurface {
public:
	BaseSurfaceOpenGL(BaseGame *inGame);
	~BaseSurfaceOpenGL();

	bool create(const Common::String &filename, bool defaultCK, byte ckRed, byte ckGreen, byte ckBlue, int lifeTime = -1, bool keepLoaded = false) override;
	bool create(int width, int height) override;

	bool isTransparentAt(int x, int y) override;
	bool isTransparentAtLite(int x, int y) override;

	bool startPixelOp() override;
	bool endPixelOp() override;

	virtual bool putSurface(const Graphics::Surface &surface, bool hasAlpha = false) override;

	virtual int getWidth() override {
		if (!_loaded) {
			finishLoad();
		}
		if (_surface) {
			return _surface->w;
		}
		return _width;
	}
	virtual int getHeight() override {
		if (!_loaded) {
			finishLoad();
		}
		if (_surface) {
			return _surface->h;
		}
		return _height;
	}

	Graphics::AlphaType getAlphaType() const {
		return _alphaType;
	}
private:
	Graphics::Surface *_surface;
	Common::SharedPtr<Texture> _texture;
	bool _loaded;
	bool finishLoad();
	bool drawSprite(int x, int y, Rect32 *rect, Rect32 *newRect, Graphics::TransformStruct transformStruct);
	void genAlphaMask(Graphics::Surface *surface);
	uint32 getPixelAt(Graphics::Surface *surface, int x, int y);

	Graphics::AlphaType _alphaType;
	void *_lockPixels;
	int _lockPitch;
	byte *_alphaMask;
};

} // End of namespace Wintermute

#endif
