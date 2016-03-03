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

#include "engines/wintermute/wintypes.h"
#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/gfx/base_surface.h"

namespace Wintermute {

//////////////////////////////////////////////////////////////////////
BaseSurface::BaseSurface(BaseGame *inGame) : BaseClass(inGame) {
	_referenceCount = 0;

	_width = _height = 0;

	_filename = "";

	_pixelOpReady = false;

	_ckDefault = true;
	_ckRed = _ckGreen = _ckBlue = 0;
	_lifeTime = 0;
	_keepLoaded = false;

	_lastUsedTime = 0;
	_valid = false;
	_rotation = 0;
}


//////////////////////////////////////////////////////////////////////
BaseSurface::~BaseSurface() {
	if (_pixelOpReady) {
		endPixelOp();
	}
}

//////////////////////////////////////////////////////////////////////
bool BaseSurface::restore() {
	return STATUS_FAILED;
}

//////////////////////////////////////////////////////////////////////
bool BaseSurface::isTransparentAt(int x, int y) {
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::display(int x, int y, Rect32 rect, Graphics::TSpriteBlendMode blendMode, bool mirrorX, bool mirrorY) {
	_rotation = 0;
	return drawSprite(x, y, &rect, nullptr, Graphics::TransformStruct(Graphics::kDefaultZoomX, Graphics::kDefaultZoomY,  mirrorX, mirrorY));
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::displayTrans(int x, int y, Rect32 rect, uint32 alpha, Graphics::TSpriteBlendMode blendMode, bool mirrorX, bool mirrorY) {
	_rotation = 0;
	return drawSprite(x, y, &rect, nullptr, Graphics::TransformStruct(Graphics::kDefaultZoomX, Graphics::kDefaultZoomY, blendMode, alpha, mirrorX, mirrorY));
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::displayTransOffset(int x, int y, Rect32 rect, uint32 alpha, Graphics::TSpriteBlendMode blendMode, bool mirrorX, bool mirrorY, int offsetX, int offsetY) {
	_rotation = 0;
	return drawSprite(x, y, &rect, nullptr,  Graphics::TransformStruct(Graphics::kDefaultZoomX, Graphics::kDefaultZoomY, Graphics::kDefaultAngle, Graphics::kDefaultHotspotX, Graphics::kDefaultHotspotY, blendMode, alpha, mirrorX, mirrorY, offsetX, offsetY));
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::displayTransZoom(int x, int y, Rect32 rect, float zoomX, float zoomY, uint32 alpha, Graphics::TSpriteBlendMode blendMode, bool mirrorX, bool mirrorY) {
	_rotation = 0;
	return drawSprite(x, y, &rect, nullptr, Graphics::TransformStruct((int32)zoomX, (int32)zoomY, blendMode, alpha, mirrorX, mirrorY));
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::displayZoom(int x, int y, Rect32 rect, float zoomX, float zoomY, uint32 alpha, bool transparent, Graphics::TSpriteBlendMode blendMode, bool mirrorX, bool mirrorY) {
	_rotation = 0;
	Graphics::TransformStruct transform;
	if (transparent) {
		transform = Graphics::TransformStruct((int32)zoomX, (int32)zoomY, Graphics::kDefaultAngle, Graphics::kDefaultHotspotX, Graphics::kDefaultHotspotY, blendMode, alpha,  mirrorX, mirrorY);
	} else {
		transform = Graphics::TransformStruct((int32)zoomX, (int32)zoomY, mirrorX, mirrorY);
	}
	return drawSprite(x, y, &rect, nullptr, transform);
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::displayTransform(int x, int y, Rect32 rect, Rect32 newRect, const Graphics::TransformStruct &transform) {
	_rotation = (uint32)transform._angle;
	if (transform._angle < 0.0f) {
		warning("Negative rotation: %d %d", transform._angle, _rotation);
		_rotation = (uint32)(360.0f + transform._angle);
		warning("Negative post rotation: %d %d", transform._angle, _rotation);
	}
	return drawSprite(x, y, &rect, &newRect, transform);
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::displayTiled(int x, int y, Rect32 rect, int numTimesX, int numTimesY) {
	assert(numTimesX > 0 && numTimesY > 0);
	Graphics::TransformStruct transform(numTimesX, numTimesY);
	return drawSprite(x, y, &rect, nullptr, transform);
}

//////////////////////////////////////////////////////////////////////
bool BaseSurface::displayHalfTrans(int x, int y, Rect32 rect) {
	return STATUS_FAILED;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::create(int width, int height) {
	return STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::startPixelOp() {
	return STATUS_FAILED;
}

//////////////////////////////////////////////////////////////////////////
bool BaseSurface::endPixelOp() {
	return STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::getPixel(int x, int y, byte *r, byte *g, byte *b, byte *a) {
	return STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::putPixel(int x, int y, byte r, byte g, byte b, int a) {
	return STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::comparePixel(int x, int y, byte r, byte g, byte b, int a) {
	return false;
}


//////////////////////////////////////////////////////////////////////
bool BaseSurface::isTransparentAtLite(int x, int y) {
	return false;
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurface::invalidate() {
	return STATUS_FAILED;
}



//////////////////////////////////////////////////////////////////////////
bool BaseSurface::prepareToDraw() {
	_lastUsedTime = _gameRef->getLiveTimer()->getTime();

	if (!_valid) {
		//_gameRef->LOG(0, "Reviving: %s", _filename);
		return create(_filename.c_str(), _ckDefault, _ckRed, _ckGreen, _ckBlue, _lifeTime, _keepLoaded);
	} else {
		return STATUS_OK;
	}
}


//////////////////////////////////////////////////////////////////////////
void BaseSurface::setSize(int width, int height) {
	_width = width;
	_height = height;
}

} // End of namespace Wintermute
