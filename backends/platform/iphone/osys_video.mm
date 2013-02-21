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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// Disable symbol overrides so that we can use system headers.
#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/endian.h"
#include "common/types.h"
#include "osys_main.h"
#include "iphone_video.h"

#include "graphics/conversion.h"
#include "graphics/pixelbuffer.h"
#include "texture.h"

void OSystem_IPHONE::initVideoContext() {
	_videoContext = [g_iPhoneViewInstance getVideoContext];
	[g_iPhoneViewInstance createThreadContext];
	GLESTexture::initGL();

}

void OSystem_IPHONE::initSurface() {
	if (_gameTexture)
		_gameTexture->reinit();
	
	if (_overlayTexture) {
		_overlayTexture->reinit();
		initOverlay();
	}

	if (_mouseTexture)
		_mouseTexture->reinit();

}

void OSystem_IPHONE::initOverlay() {
	int overlayWidth = MAX([g_iPhoneViewInstance getRenderBufferWidth], 320);
	int overlayHeight = MAX([g_iPhoneViewInstance getRenderBufferHeight], 200);
	_overlayTexture->allocBuffer(overlayWidth, overlayHeight);
	_overlayTexture->setDrawRect(0, 0, [g_iPhoneViewInstance getRenderBufferHeight], [g_iPhoneViewInstance getRenderBufferHeight]);
}

const OSystem::GraphicsMode *OSystem_IPHONE::getSupportedGraphicsModes() const {
	return s_supportedGraphicsModes;
}

int OSystem_IPHONE::getDefaultGraphicsMode() const {
	return kGraphicsModeLinear;
}

bool OSystem_IPHONE::setGraphicsMode(int mode) {
	switch (mode) {
	case kGraphicsModeNone:
	case kGraphicsModeLinear:
		_videoContext->graphicsMode = (GraphicsModes)mode;
		return true;

	default:
		return false;
	}
}

int OSystem_IPHONE::getGraphicsMode() const {
	return _videoContext->graphicsMode;
}

void OSystem_IPHONE::initViewport() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// TODO:
	//glViewport(0, 0, [g_iPhoneViewInstance getRenderBufferHeight], [g_iPhoneViewInstance getRenderBufferWidth]);
	//glViewport(0, [g_iPhoneViewInstance getRenderBufferHeight] - 480, 640, 480);
	glViewport(0, 0, 768, 1024);
	warning("width: %d height: %d", [g_iPhoneViewInstance getRenderBufferWidth],[g_iPhoneViewInstance getRenderBufferHeight]);
	warning("glViewPort(%d, %d, %d, %d)", 0, [g_iPhoneViewInstance getRenderBufferHeight] - 480, 640, 480);
}

#ifdef USE_RGB_COLOR
Common::String OSystem_IPHONE::getPixelFormatName(const Graphics::PixelFormat &format) const {
	if (format.bytesPerPixel == 1)
		return "CLUT8";
	
	if (format.aLoss == 8)
		return Common::String::format("RGB%u%u%u",
									  8 - format.rLoss,
									  8 - format.gLoss,
									  8 - format.bLoss);
	
	return Common::String::format("RGBA%u%u%u%u",
								  8 - format.rLoss,
								  8 - format.gLoss,
								  8 - format.bLoss,
								  8 - format.aLoss);
}

void OSystem_IPHONE::initTexture(GLESBaseTexture **texture,
								 uint width, uint height,
								 const Graphics::PixelFormat *format) {
	assert(texture);
	Graphics::PixelFormat formatCLUT8 =
	Graphics::PixelFormat::createFormatCLUT8();
	Graphics::PixelFormat formatCurrent;
	Graphics::PixelFormat formatNew;
	
	if (*texture)
		formatCurrent = (*texture)->getPixelFormat();
	else
		formatCurrent = Graphics::PixelFormat();
	
	if (format)
		formatNew = *format;
	else
		formatNew = formatCLUT8;
	
	if (formatCurrent != formatNew) {
		if (*texture)
			warning("switching pixel format from: %s",
					getPixelFormatName((*texture)->getPixelFormat()).c_str());
		
		delete *texture;
		
		if (formatNew == GLES565Texture::pixelFormat())
			*texture = new GLES565Texture();
		else if (formatNew == GLES5551Texture::pixelFormat())
			*texture = new GLES5551Texture();
		else if (formatNew == GLES4444Texture::pixelFormat())
			*texture = new GLES4444Texture();
		else {
			// TODO what now?
			if (formatNew != formatCLUT8)
				error("unsupported pixel format: %s",
					  getPixelFormatName(formatNew).c_str());
			
			*texture = new GLESFakePalette565Texture;
		}
		
		warning("new pixel format: %s",
				getPixelFormatName((*texture)->getPixelFormat()).c_str());
	}
	
	(*texture)->allocBuffer(width, height);
}

Common::List<Graphics::PixelFormat> OSystem_IPHONE::getSupportedFormats() const {
	Common::List<Graphics::PixelFormat> list;
	// RGB565
	list.push_back(Graphics::createPixelFormat<565>());
	// CLUT8
	list.push_back(Graphics::PixelFormat::createFormatCLUT8());
	return list;
}
#endif

void OSystem_IPHONE::launcherInitSize(uint width, uint height) {
	setupScreen(width, height, true, true, false);
}

void OSystem_IPHONE::updateEventScale() {
	const GLESBaseTexture *tex;
	
	if (_videoContext->overlayVisible)
		tex = _overlayTexture;
	else
		tex = _gameTexture;
	
	float _eventScaleY = 100 * 480 / tex->height();
	float _eventScaleX = 100 * 640 / tex->width();
	warning("TODO: Make updateEventScale do something");
}

void OSystem_IPHONE::initSize(uint width, uint height, const Graphics::PixelFormat *format) {
//	ENTER("%d, %d, %p", width, height, format);
	
//	GLTHREADCHECK;
	
#ifdef USE_RGB_COLOR
	initTexture(&_gameTexture, width, height, format);
#else
	_game_texture->allocBuffer(width, height);
#endif
	
	updateScreenRect();
	updateEventScale();
	
	// Don't know mouse size yet - it gets reallocated in
	// setMouseCursor.  We need the palette allocated before
	// setMouseCursor however, so just take a guess at the desired
	// size (it's small).
	_mouseTexturePalette->allocBuffer(20, 20);
	
	clearScreen(kClear);
}

Graphics::PixelBuffer OSystem_IPHONE::setupScreen(int width, int height, bool fullscreen, bool accel3d, bool isGame) {
	printf("Setup 3d: %d", _opengl);
	//printf("initSize(%u, %u, %p)\n", width, height, (const void *)format);
	_opengl = accel3d;
	initViewport();
	Graphics::PixelFormat pFormat = Graphics::createPixelFormat<565>();
	Graphics::PixelFormat *format = &pFormat;

#ifdef USE_RGB_COLOR
	initTexture(&_gameTexture, width, height, format);
#else
	_gameTexture->allocBuffer(width, height);
#endif

	_videoContext->screenWidth = width;
	_videoContext->screenHeight = height;
	_videoContext->shakeOffsetY = 0;

	// Create the screen texture right here. We need to do this here, since
	// when a game requests hi-color mode, we actually set the framebuffer
	// to the texture buffer to avoid an additional copy step.

	_fullScreenIsDirty = false;
	dirtyFullScreen();
	_mouseCursorPaletteEnabled = false;
	Graphics::Surface screenSurface;
//	_pixelBuffer.set(pFormat, (byte *)_residualVMFrame.pixels);
	
	if (accel3d) {
		// resize game texture
		initSize(width, height, 0);
		if (isGame)
			_gameTexture->setGameTexture();
		// format is not used by the gfx_opengl driver, use fake format
		_gamePbuf.set(Graphics::PixelFormat(), 0);
		
	} else {
		Graphics::PixelFormat format = GLES565Texture::pixelFormat();
		initSize(width, height, &format);
		// as there is no support for the texture surface's lock/unlock mechanism in gfx_tinygl/...
		// do not use _game_texture->surface()->pixels directly
		_gamePbuf.create(_gameTexture->getPixelFormat(), _gameTexture->width() * _gameTexture->height(), getDisposeAfterUseYes());
	}
	return _gamePbuf;
}

void OSystem_IPHONE::beginGFXTransaction() {
	_gfxTransactionError = kTransactionSuccess;
}

#define GLCALL(x) x

void OSystem_IPHONE::clearScreen(FixupType type, byte count) {
	assert(count > 0);
	
	bool sm = _showMouse;
	_showMouse = false;
	
	GLCALL(glDisable(GL_SCISSOR_TEST));
	
	for (byte i = 0; i < count; ++i) {
		// clear screen
		GLCALL(glClearColor(0, 0, 0, 1 << 16));
		if (_opengl) {
			GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		} else {
			GLCALL(glClear(GL_COLOR_BUFFER_BIT));
		}
		
		switch (type) {
			case kClear:
				break;
				
			case kClearSwap:
				warning("TODO: Swap buffers");
//				JNI::swapBuffers();
				break;
				
			case kClearUpdate:
				_forceRedraw = true;
				updateScreen();
				break;
		}
	}

	if (!_videoContext->overlayVisible)
		GLCALL(glEnable(GL_SCISSOR_TEST));
	
	_showMouse = sm;
	_forceRedraw = true;
}

OSystem::TransactionError OSystem_IPHONE::endGFXTransaction() {
	_screenChangeCount++;
	updateOutputSurface();
	[g_iPhoneViewInstance setGraphicsMode];

	return _gfxTransactionError;
}

void OSystem_IPHONE::updateOutputSurface() {
	[g_iPhoneViewInstance initSurface];
}

int16 OSystem_IPHONE::getHeight() {
	return _videoContext->screenHeight;
}

int16 OSystem_IPHONE::getWidth() {
	return _videoContext->screenWidth;
}

void OSystem_IPHONE::setPalette(const byte *colors, uint start, uint num) {
	//printf("setPalette(%p, %u, %u)\n", colors, start, num);
	assert(start + num <= 256);
	const byte *b = colors;

	for (uint i = start; i < start + num; ++i) {
		_gamePalette[i] = Graphics::RGBToColor<Graphics::ColorMasks<565> >(b[0], b[1], b[2]);
		_gamePaletteRGBA5551[i] = Graphics::RGBToColor<Graphics::ColorMasks<5551> >(b[0], b[1], b[2]);
		b += 3;
	}

	dirtyFullScreen();

	// Automatically update the mouse texture when the palette changes while the
	// cursor palette is disabled.
	if (!_mouseCursorPaletteEnabled && _mouseBuffer.format.bytesPerPixel == 1)
		_mouseDirty = _mouseNeedTextureUpdate = true;
}

void OSystem_IPHONE::grabPalette(byte *colors, uint start, uint num) {
	//printf("grabPalette(%p, %u, %u)\n", colors, start, num);
	assert(start + num <= 256);
	byte *b = colors;

	for (uint i = start; i < start + num; ++i) {
		Graphics::colorToRGB<Graphics::ColorMasks<565> >(_gamePalette[i], b[0], b[1], b[2]);
		b += 3;
	}
}

void OSystem_IPHONE::copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) {
	//printf("copyRectToScreen(%p, %d, %i, %i, %i, %i)\n", buf, pitch, x, y, w, h);
	//Clip the coordinates
//	ENTER("%p, %d, %d, %d, %d, %d", buf, pitch, x, y, w, h);
	
//	GLTHREADCHECK;
	
	_gameTexture->updateBuffer(x, y, w, h, buf, pitch);
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(const char *file, int line);

void OSystem_IPHONE::updateScreen() {

	if (_gamePbuf) {
		int pitch = _gameTexture->width() * _gameTexture->getPixelFormat().bytesPerPixel;
		_gameTexture->updateBuffer(0, 0, _gameTexture->width(), _gameTexture->height(),
									_gamePbuf.getRawBuffer(), pitch);
	}
	#if 0
	_gameTexture->surface();
	_gameTexture->drawTextureRect();
#endif
	if (_videoContext->overlayVisible) {
		_overlayTexture->surface();
		_overlayTexture->drawTextureRect();
	}
	if (_videoContext->overlayVisible) {
		Common::Point mouse;
		int cs = 1;
		mouse.x = _videoContext->mouseX;
		mouse.y = _videoContext->mouseY;
		_mouseTexture->drawTexture(mouse.x * cs, mouse.y * cs, _mouseTexture->width(), _mouseTexture->height());
	}

	iPhone_updateScreen();
	return;
}

void OSystem_IPHONE::updateScreenRect() {
	Common::Rect rect(0, 0, [g_iPhoneViewInstance getRenderBufferWidth], [g_iPhoneViewInstance getRenderBufferHeight]);
	
	_overlayTexture->setDrawRect(rect);
	
	uint16 w = _gameTexture->width();
	uint16 h = _gameTexture->height();
	
	if (w && h && !_fullscreen) {
		if (_videoContext->asprectRatioCorrection && w == 320 && h == 200)
			h = 240;
		
		float dpi[2] = {320.0f, 320.0f}; // TODO: Retina
		//JNI::getDPI(dpi);
		
		float screen_ar;
		if (dpi[0] != 0.0 && dpi[1] != 0.0) {
			// horizontal orientation
			screen_ar = (dpi[1] * [g_iPhoneViewInstance getRenderBufferWidth]) /
			(dpi[0] * [g_iPhoneViewInstance getRenderBufferHeight]);
		} else {
			screen_ar = float([g_iPhoneViewInstance getRenderBufferWidth]) / float([g_iPhoneViewInstance getRenderBufferHeight]);
		}
		
		float game_ar = float(w) / float(h);
		
		if (screen_ar > game_ar) {
			rect.setWidth(round([g_iPhoneViewInstance getRenderBufferHeight] * game_ar));
			rect.moveTo(([g_iPhoneViewInstance getRenderBufferWidth] - rect.width()) / 2, 0);
		} else {
			rect.setHeight(round([g_iPhoneViewInstance getRenderBufferWidth] / game_ar));
			rect.moveTo(([g_iPhoneViewInstance getRenderBufferHeight] - rect.height()) / 2, 0);
		}
	}
	
	glScissor(rect.left, rect.top, rect.width(), rect.height());
	
	_gameTexture->setDrawRect(rect);
}

void OSystem_IPHONE::internUpdateScreen() {
	if (_mouseNeedTextureUpdate) {
		updateMouseTexture();
		_mouseNeedTextureUpdate = false;
	}

	while (_dirtyRects.size()) {
		Common::Rect dirtyRect = _dirtyRects.remove_at(_dirtyRects.size() - 1);

		//printf("Drawing: (%i, %i) -> (%i, %i)\n", dirtyRect.left, dirtyRect.top, dirtyRect.right, dirtyRect.bottom);
		drawDirtyRect(dirtyRect);
		// TODO: Implement dirty rect code
		//updateHardwareSurfaceForRect(dirtyRect);
	}

	if (_videoContext->overlayVisible) {
		// TODO: Implement dirty rect code
		_dirtyOverlayRects.clear();
		/*while (_dirtyOverlayRects.size()) {
			Common::Rect dirtyRect = _dirtyOverlayRects.remove_at(_dirtyOverlayRects.size() - 1);

			//printf("Drawing: (%i, %i) -> (%i, %i)\n", dirtyRect.left, dirtyRect.top, dirtyRect.right, dirtyRect.bottom);
			drawDirtyOverlayRect(dirtyRect);
		}*/
	}
}

void OSystem_IPHONE::drawDirtyRect(const Common::Rect &dirtyRect) { // Not used
}

Graphics::Surface *OSystem_IPHONE::lockScreen() {
	//printf("lockScreen()\n");
//	ENTER();
	
//	GLTHREADCHECK;
	Graphics::Surface *surface = _gameTexture->surface();
	assert(surface->pixels);
	
	return surface;

}

void OSystem_IPHONE::unlockScreen() {
	//printf("unlockScreen()\n");
//	ENTER();
	
//	GLTHREADCHECK;
	
	assert(_gameTexture->dirty());
}

void OSystem_IPHONE::setShakePos(int shakeOffset) {
	//printf("setShakePos(%i)\n", shakeOffset);
	_videoContext->shakeOffsetY = shakeOffset;
	[g_iPhoneViewInstance setViewTransformation];
	// HACK: We use this to force a redraw.
	_mouseDirty = true;
}

void OSystem_IPHONE::showOverlay() {
	//printf("showOverlay()\n");
	_videoContext->overlayVisible = true;
	dirtyFullOverlayScreen();
	updateScreen();
	[g_iPhoneViewInstance updateMouseCursorScaling];
	[g_iPhoneViewInstance clearColorBuffer];
}

void OSystem_IPHONE::hideOverlay() {
	//printf("hideOverlay()\n");
	_videoContext->overlayVisible = false;
	_dirtyOverlayRects.clear();
	dirtyFullScreen();
	[g_iPhoneViewInstance updateMouseCursorScaling];
	[g_iPhoneViewInstance clearColorBuffer];
}

void OSystem_IPHONE::clearOverlay() {
	//printf("clearOverlay()\n");
	_overlayTexture->fillBuffer(0);
//	bzero(_videoContext->overlayTexture.getBasePtr(0, 0), _videoContext->overlayTexture.h * _videoContext->overlayTexture.pitch);
	dirtyFullOverlayScreen();
}

void OSystem_IPHONE::grabOverlay(void *buf, int pitch) { // TODO
	//printf("grabOverlay()\n");
	int h = _videoContext->overlayHeight;

/*	byte *dst = (byte *)buf;
	const byte *src = (const byte *)_videoContext->overlayTexture.getBasePtr(0, 0);
	do {
		memcpy(dst, src, _videoContext->overlayWidth * sizeof(uint16));
		src += _videoContext->overlayTexture.pitch;
		dst += pitch;
	} while (--h);*/
}

void OSystem_IPHONE::copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) {
	//printf("copyRectToOverlay(%p, pitch=%i, x=%i, y=%i, w=%i, h=%i)\n", (const void *)buf, pitch, x, y, w, h);
	_overlayTexture->updateBuffer(x, y, w, h, buf, pitch);
	return;
}

int16 OSystem_IPHONE::getOverlayHeight() {
	return _videoContext->overlayHeight;
}

int16 OSystem_IPHONE::getOverlayWidth() {
	return _videoContext->overlayWidth;
}

bool OSystem_IPHONE::showMouse(bool visible) {
	//printf("showMouse(%d)\n", visible);
	bool last = _videoContext->mouseIsVisible;
	_videoContext->mouseIsVisible = visible;
	_mouseDirty = true;

	return last;
}

void OSystem_IPHONE::warpMouse(int x, int y) {
	//printf("warpMouse(%d, %d)\n", x, y);
	_videoContext->mouseX = x;
	_videoContext->mouseY = y;
	[g_iPhoneViewInstance notifyMouseMove];
	_mouseDirty = true;
}

void OSystem_IPHONE::dirtyFullScreen() {
	if (!_fullScreenIsDirty) {
		_dirtyRects.clear();
		_dirtyRects.push_back(Common::Rect(0, 0, _videoContext->screenWidth, _videoContext->screenHeight));
		_fullScreenIsDirty = true;
	}
}

void OSystem_IPHONE::dirtyFullOverlayScreen() {
	if (!_fullScreenOverlayIsDirty) {
		_dirtyOverlayRects.clear();
		_dirtyOverlayRects.push_back(Common::Rect(0, 0, _videoContext->overlayWidth, _videoContext->overlayHeight));
		_fullScreenOverlayIsDirty = true;
	}
}

void OSystem_IPHONE::setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, bool dontScale, const Graphics::PixelFormat *format) {
	//printf("setMouseCursor(%p, %u, %u, %i, %i, %u, %d, %p)\n", (const void *)buf, w, h, hotspotX, hotspotY, keycolor, dontScale, (const void *)format);
/*
	const Graphics::PixelFormat pixelFormat = format ? *format : Graphics::PixelFormat::createFormatCLUT8();
#if 0
	printf("bytesPerPixel: %u RGBAlosses: %u,%u,%u,%u RGBAshifts: %u,%u,%u,%u\n", pixelFormat.bytesPerPixel,
	       pixelFormat.rLoss, pixelFormat.gLoss, pixelFormat.bLoss, pixelFormat.aLoss,
	       pixelFormat.rShift, pixelFormat.gShift, pixelFormat.bShift, pixelFormat.aShift);
#endif
	assert(pixelFormat.bytesPerPixel == 1 || pixelFormat.bytesPerPixel == 2);

	if (_mouseBuffer.w != w || _mouseBuffer.h != h || _mouseBuffer.format != pixelFormat || !_mouseBuffer.pixels)
		_mouseBuffer.create(w, h, pixelFormat);

	_videoContext->mouseWidth = w;
	_videoContext->mouseHeight = h;

	_videoContext->mouseHotspotX = hotspotX;
	_videoContext->mouseHotspotY = hotspotY;

	_mouseKeyColor = keycolor;

	memcpy(_mouseBuffer.getBasePtr(0, 0), buf, h * _mouseBuffer.pitch);

	_mouseDirty = true;
	_mouseNeedTextureUpdate = true;*/
	
#ifdef USE_RGB_COLOR
// TODO:
/*
	if (format && format->bytesPerPixel > 1) {
		if (_mouse_texture != _mouse_texture_rgb) {
			LOGD("switching to rgb mouse cursor");
			
			assert(!_mouse_texture_rgb);
			_mouse_texture_rgb = new GLES5551Texture();
			_mouse_texture_rgb->setLinearFilter(_graphicsMode == 1);
		}
		
		_mouse_texture = _mouse_texture_rgb;
	} else {
		if (_mouse_texture != _mouse_texture_palette)
			LOGD("switching to paletted mouse cursor");
		
		_mouse_texture = _mouse_texture_palette;
		
		delete _mouse_texture_rgb;
		_mouse_texture_rgb = 0;
	}
	*/
#endif
	
	_mouseTexture->allocBuffer(w, h);
	
	if (_mouseTexture == _mouseTexturePalette) {
		assert(keycolor < 256);
		
		byte *p = _mouseTexturePalette->palette() + _mouseKeycolor * 2;
		WRITE_UINT16(p, READ_UINT16(p) | 1);
		
		_mouseKeycolor = keycolor;
		
		p = _mouseTexturePalette->palette() + _mouseKeycolor * 2;
		WRITE_UINT16(p, READ_UINT16(p) & ~1);
	}
	
	if (w == 0 || h == 0)
		return;
	
	if (_mouseTexture == _mouseTexturePalette) {
		_mouseTexture->updateBuffer(0, 0, w, h, buf, w);
	} else {
		uint16 pitch = _mouseTexture->pitch();
		
		byte *tmp = new byte[pitch * h];
		
		// meh, a 16bit cursor without alpha bits... this is so silly
		if (!crossBlit(tmp, (const byte *)buf, pitch, w * 2, w, h,
					   _mouseTexture->getPixelFormat(),
					   *format)) {
			warning("crossblit failed");
			
			delete[] tmp;
			
			_mouseTexture->allocBuffer(0, 0);
			
			return;
		}
		
		uint16 *s = (uint16 *)buf;
		uint16 *d = (uint16 *)tmp;
		for (uint16 y = 0; y < h; ++y, d += pitch / 2 - w)
			for (uint16 x = 0; x < w; ++x, d++)
				if (*s++ != (keycolor & 0xffff))
					*d |= 1;
		
		_mouseTexture->updateBuffer(0, 0, w, h, tmp, pitch);
		
		delete[] tmp;
	}
	
	_mouseHotspot = Common::Point(hotspotX, hotspotY);
	// TODO: Adapt to the new "do not scale" cursor logic.
	_mouseTargetscale = 1;
}

void OSystem_IPHONE::setCursorPaletteInternal(const byte *colors, uint start, uint num) {
	const Graphics::PixelFormat &pf =
	_mouseTexturePalette->getPalettePixelFormat();
	byte *p = _mouseTexturePalette->palette() + start * 2;
	
	for (uint i = 0; i < num; ++i, colors += 3, p += 2)
		WRITE_UINT16(p, pf.RGBToColor(colors[0], colors[1], colors[2]));
	
	p = _mouseTexturePalette->palette() + _mouseKeycolor * 2;
	WRITE_UINT16(p, READ_UINT16(p) & ~1);
}

void OSystem_IPHONE::setCursorPalette(const byte *colors, uint start, uint num) {
//	ENTER("%p, %u, %u", colors, start, num);
	
//	GLTHREADCHECK;
	
	if (!_mouseTexture->hasPalette()) {
		warning("switching to paletted mouse cursor");
		
		_mouseTexture = _mouseTexturePalette;
		
		delete _mouseTextureRGB;
		_mouseTextureRGB = 0;
	}
	
	setCursorPaletteInternal(colors, start, num);
	_useMousePalette = true;
}

void OSystem_IPHONE::updateMouseTexture() {
	uint texWidth = getSizeNextPOT(_videoContext->mouseWidth);
	uint texHeight = getSizeNextPOT(_videoContext->mouseHeight);

	Graphics::Surface &mouseTexture = _videoContext->mouseTexture;
	if (mouseTexture.w != texWidth || mouseTexture.h != texHeight)
		mouseTexture.create(texWidth, texHeight, Graphics::createPixelFormat<5551>());

	if (_mouseBuffer.format.bytesPerPixel == 1) {
		const uint16 *palette;
		if (_mouseCursorPaletteEnabled)
			palette = _mouseCursorPalette;
		else
			palette = _gamePaletteRGBA5551;

		uint16 *mouseBuf = (uint16 *)mouseTexture.getBasePtr(0, 0);
		for (uint x = 0; x < _videoContext->mouseWidth; ++x) {
			for (uint y = 0; y < _videoContext->mouseHeight; ++y) {
				const byte color = *(const byte *)_mouseBuffer.getBasePtr(x, y);
				if (color != _mouseKeyColor)
					mouseBuf[y * texWidth + x] = palette[color] | 0x1;
				else
					mouseBuf[y * texWidth + x] = 0x0;
			}
		}
	} else {
		if (crossBlit((byte *)mouseTexture.getBasePtr(0, 0), (const byte *)_mouseBuffer.getBasePtr(0, 0), mouseTexture.pitch,
			          _mouseBuffer.pitch, _mouseBuffer.w, _mouseBuffer.h, mouseTexture.format, _mouseBuffer.format)) {
			if (!_mouseBuffer.format.aBits()) {
				// Apply color keying since the original cursor had no alpha channel.
				const uint16 *src = (const uint16 *)_mouseBuffer.getBasePtr(0, 0);
				uint8 *dstRaw = (uint8 *)mouseTexture.getBasePtr(0, 0);

				for (uint y = 0; y < _mouseBuffer.h; ++y, dstRaw += mouseTexture.pitch) {
					uint16 *dst = (uint16 *)dstRaw;
					for (uint x = 0; x < _mouseBuffer.w; ++x, ++dst) {
						if (*src++ == _mouseKeyColor)
							*dst &= ~1;
						else
							*dst |= 1;
					}
				}
			}
		} else {
			// TODO: Log this!
			// Make the cursor all transparent... we really need a better fallback ;-).
			memset(mouseTexture.getBasePtr(0, 0), 0, mouseTexture.h * mouseTexture.pitch);
		}
	}
	[g_iPhoneViewInstance updateMouseCursor];
}
