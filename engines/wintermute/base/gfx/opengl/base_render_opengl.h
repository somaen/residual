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

#ifndef WINTERMUTE_BASE_RENDERER_OPENGL_H
#define WINTERMUTE_BASE_RENDERER_OPENGL_H

#ifdef USE_OPENGL

#ifdef USE_GLEW
#include <GL/glew.h>
#elif defined (SDL_BACKEND) && !defined(__amigaos4__)
#include <SDL_opengl.h>
#undef ARRAYSIZE
#else
#include <GL/gl.h>
#endif

#include "engines/wintermute/base/gfx/base_renderer.h"
#include "common/rect.h"
#include "graphics/surface.h"
#include "common/list.h"
#include "graphics/transform_struct.h"

namespace Wintermute {
class BaseSurfaceOpenGL;
class RenderTicketGL;
class Texture;
/**
 * A 2D-renderer implementation for WME.
 * This renderer makes use of a "ticket"-system, where all draw-calls
 * are stored as tickets until flip() is called, and compared against the tickets
 * from last frame, to determine which calls were the same as last round
 * (i.e. in the exact same order, with the exact same arguments), and thus
 * figure out which parts of the screen need to be redrawn.
 *
 * Important concepts to handle here, is the ordered number of any ticket
 * which is called the "drawNum", every frame this starts from scratch, and
 * then the incoming tickets created from the draw-calls are checked to see whether
 * they came before, on, or after the drawNum they had last frame. Everything else
 * being equal, this information is then used to check whether the draw order changed,
 * which will then create a need for redrawing, as we draw with an alpha-channel here.
 *
 * There is also a draw path that draws without tickets, for debugging purposes,
 * as well as to accomodate situations with large enough amounts of draw calls,
 * that there will be too much overhead involved with comparing the generated tickets.
 */
class BaseRenderOpenGL : public BaseRenderer {
public:
	BaseRenderOpenGL(BaseGame *inGame);
	~BaseRenderOpenGL();

	typedef Common::List<RenderTicketGL *>::iterator RenderQueueIterator;

	Common::String getName() const;

	bool initRenderer(int width, int height, bool windowed) override;
	bool flip() override;
	virtual bool indicatorFlip();
	bool fill(byte r, byte g, byte b, Common::Rect *rect = nullptr) override;
	Graphics::PixelFormat getPixelFormat() const override;
	void fade(uint16 alpha) override;
	void fadeToColor(byte r, byte g, byte b, byte a) override;

	bool drawLine(int x1, int y1, int x2, int y2, uint32 color) override;

	BaseImage *takeScreenshot() override;

	bool setViewport(Rect32 *rect) override {
		return BaseRenderer::setViewport(rect);
	}

	void dumpData(const char *filename) override;

	float getScaleRatioX() const override {
		return _ratioX;
	}
	float getScaleRatioY() const override {
		return _ratioY;
	}
	virtual bool startSpriteBatch() override;
	virtual bool endSpriteBatch() override;
	void endSaveLoad();

	void drawSurface(BaseSurfaceOpenGL *owner, const Graphics::Surface *surf, Common::Rect *srcRect, Common::Rect *dstRect, Graphics::TransformStruct &transform);
	void drawTexture(BaseSurfaceOpenGL *owner, const Texture &texture, Common::Rect *srcRect, Common::Rect *dstRect, Graphics::TransformStruct &transform);
	BaseSurface *createSurface() override;
private:
	bool _needsFlip;

	uint32 _clearColor;

	bool _skipThisFrame;
	int _lastScreenChangeID; // previous value of OSystem::getScreenChangeID()
};

} // End of namespace Wintermute

#endif

#endif
