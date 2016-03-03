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

#include "common/system.h"
#include "engines/wintermute/base/gfx/opengl/base_render_opengl.h"
#include "engines/wintermute/base/gfx/opengl/base_surface_opengl.h"
#include "engines/wintermute/base/gfx/opengl/textured_rect.h"
#include "engines/wintermute/base/base_surface_storage.h"
#include "engines/wintermute/base/gfx/base_image.h"
#include "engines/wintermute/math/math_util.h"
#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/base_sprite.h"
#include "graphics/transparent_surface.h"
#include "graphics/pixelbuffer.h"
#include "common/queue.h"
#include "common/config-manager.h"

#define DIRTY_RECT_LIMIT 800

namespace Wintermute {

BaseRenderer *makeOpenGLRenderer(BaseGame *inGame) {
	return new BaseRenderOpenGL(inGame);
}

//////////////////////////////////////////////////////////////////////////
BaseRenderOpenGL::BaseRenderOpenGL(BaseGame *inGame) : BaseRenderer(inGame) {
	_needsFlip = true;
	_skipThisFrame = false;

	_ratioX = _ratioY = 1.0f;

	_lastScreenChangeID = g_system->getScreenChangeID();
}

//////////////////////////////////////////////////////////////////////////
BaseRenderOpenGL::~BaseRenderOpenGL() {
}

//////////////////////////////////////////////////////////////////////////
bool BaseRenderOpenGL::initRenderer(int width, int height, bool windowed) {
	Graphics::PixelFormat format(4, 8, 8, 8, 8, 0, 8, 16, 24);
	BaseRenderer::initRenderer(width, height, windowed, format);
	/*
	    g_system->beginGFXTransaction();
	    g_system->initSize(_width, _height, &format);
	    OSystem::TransactionError gfxError = g_system->endGFXTransaction();
	    */
	Graphics::PixelFormat pixelFormat = g_system->setupScreen(width, height, !windowed, true).getFormat();
	/*
	    if (gfxError != OSystem::kTransactionSuccess) {
	        warning("Couldn't setup GFX-backend for %dx%dx%d", _width, _height, format.bytesPerPixel * 8);
	        return STATUS_FAILED;
	    }
	*/
	g_system->showMouse(false);

	_active = true;

//	_clearColor = _renderSurface->format.ARGBToColor(255, 0, 0, 0);


	glViewport(0, 0, width, height);
	_active = true;
	_ready = true;
	_width = width;
	_height = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glScalef(0, -1, 0);
	glLoadIdentity();


	return STATUS_OK;
}

bool BaseRenderOpenGL::indicatorFlip() {
	warning("TODO: IndicatorFlip()");
/*	g_system->copyRectToScreen((byte *)_renderSurface->getBasePtr(_indicatorX, _indicatorY), _renderSurface->pitch, _indicatorX, _indicatorY, _indicatorWidthDrawn, _indicatorHeight);*/
	g_system->updateScreen();
	return STATUS_OK;
}

bool BaseRenderOpenGL::flip() {
//	warning("FLIP");
	g_system->updateScreen();
	glClear(GL_COLOR_BUFFER_BIT);
	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
bool BaseRenderOpenGL::fill(byte r, byte g, byte b, Common::Rect *rect) {
	// TODO
	glClearColor(r, g, b, 1.0f);
	return STATUS_OK;
//	_clearColor = _renderSurface->format.ARGBToColor(0xFF, r, g, b);
}

//////////////////////////////////////////////////////////////////////////
void BaseRenderOpenGL::fade(uint16 alpha) {
	byte dwAlpha = (byte)(255 - alpha);
	return fadeToColor(0, 0, 0, dwAlpha);
}

//////////////////////////////////////////////////////////////////////////
void BaseRenderOpenGL::fadeToColor(byte r, byte g, byte b, byte a) {
	Common::Rect fillRect;
	warning("TODO: fadeToColor");
	return;

	Rect32 rc;
	_gameRef->getCurrentViewportRect(&rc);
	fillRect.left = (int16)rc.left;
	fillRect.top = (int16)rc.top;
	fillRect.setWidth((int16)(rc.right - rc.left));
	fillRect.setHeight((int16)(rc.bottom - rc.top));

	modTargetRect(&fillRect);

	//TODO: This is only here until I'm sure about the final pixelformat
//	uint32 col = _renderSurface->format.ARGBToColor(a, r, g, b);
	uint32 col = 0;

	Graphics::Surface surf;
//	surf.create((uint16)fillRect.width(), (uint16)fillRect.height(), _renderSurface->format);
	Common::Rect sizeRect(fillRect);
	sizeRect.translate(-fillRect.top, -fillRect.left);
	surf.fillRect(fillRect, col);
	Graphics::TransformStruct temp = Graphics::TransformStruct();
	temp._alphaDisable = false;
	drawSurface(nullptr, &surf, &sizeRect, &fillRect, temp);
	surf.free();

	warning("FadeToColor(%d %d %d %d)", r, g, b, a);

	//SDL_SetRenderDrawColor(_renderer, r, g, b, a);
	//SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	//SDL_RenderFillRect(_renderer, &fillRect);
}

Graphics::PixelFormat BaseRenderOpenGL::getPixelFormat() const {
//	return _renderSurface->format;
	return _screenFormat;
}

void BaseRenderOpenGL::drawTexture(BaseSurfaceOpenGL *owner, const Texture &texture, Common::Rect *srcRect, Common::Rect *dstRect, Graphics::TransformStruct &transform) {
	TexturedRect src(texture);
	src.draw(*srcRect, *dstRect, transform);
}


void BaseRenderOpenGL::drawSurface(BaseSurfaceOpenGL *owner, const Graphics::Surface *surf, Common::Rect *srcRect, Common::Rect *dstRect, Graphics::TransformStruct &transform) {
	error("DRAW SURFACE");
	Texture tex(*surf);
	TexturedRect src(tex);
	src.draw(*srcRect, *dstRect, transform);

	return;
}

//////////////////////////////////////////////////////////////////////////
bool BaseRenderOpenGL::drawLine(int x1, int y1, int x2, int y2, uint32 color) {
	// This function isn't used outside of indicator-displaying, and thus quite unused in
	// BaseRenderOpenGL when dirty-rects are enabled.
	if (!_indicatorDisplay) {
		error("BaseRenderOpenGL::DrawLine - doesn't work for dirty rects yet");
	}

	byte r = RGBCOLGetR(color);
	byte g = RGBCOLGetG(color);
	byte b = RGBCOLGetB(color);
	byte a = RGBCOLGetA(color);

	//SDL_SetRenderDrawColor(_renderer, r, g, b, a);
	//SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

	Point32 point1, point2;
	point1.x = x1;
	point1.y = y1;
	pointToScreen(&point1);

	point2.x = x2;
	point2.y = y2;
	pointToScreen(&point2);

	warning("TODO: drawLine");

	// TODO: This thing is mostly here until I'm sure about the final color-format.
//	uint32 colorVal = _renderSurface->format.ARGBToColor(a, r, g, b);
//	_renderSurface->drawLine(point1.x, point1.y, point2.x, point2.y, colorVal);
	//SDL_RenderDrawLine(_renderer, point1.x, point1.y, point2.x, point2.y);
	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
BaseImage *BaseRenderOpenGL::takeScreenshot() {
// TODO: Clip by viewport.
	BaseImage *screenshot = new BaseImage();
	Graphics::Surface temp;
	temp.create(16, 16, Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0));
	screenshot->copyFrom(&temp);
	warning("TODO:takeScreenshot");
//	screenshot->copyFrom(_renderSurface);
	temp.free();
	return screenshot;
}

//////////////////////////////////////////////////////////////////////////
Common::String BaseRenderOpenGL::getName() const {
	return "ScummVM-OSystem-renderer";
}

//////////////////////////////////////////////////////////////////////////
void BaseRenderOpenGL::dumpData(const char *filename) {
	warning("BaseRenderOpenGL::DumpData(%s) - stubbed", filename); // TODO
}

BaseSurface *BaseRenderOpenGL::createSurface() {
	return new BaseSurfaceOpenGL(_gameRef);
}

void BaseRenderOpenGL::endSaveLoad() {
	BaseRenderer::endSaveLoad();

	// HACK: After a save the buffer will be drawn before the scripts get to update it,
	// so just skip this single frame.
	_skipThisFrame = true;

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	g_system->updateScreen();
}

bool BaseRenderOpenGL::startSpriteBatch() {
	return STATUS_OK;
}

bool BaseRenderOpenGL::endSpriteBatch() {
	return STATUS_OK;
}

} // End of namespace Wintermute
