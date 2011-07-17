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

#ifndef BACKENDS_GRAPHICS_SDL_SDLGRAPHICS_H
#define BACKENDS_GRAPHICS_SDL_SDLGRAPHICS_H

#include "common/rect.h"

class SdlEventSource;

/**
 * Base class for a SDL based graphics manager.
 *
 * It features a few extra a few extra features required by SdlEventSource.
 * FIXME/HACK:
 * Note it does not inherit from GraphicsManager to avoid a diamond inheritance
 * in the current OpenGLSdlGraphicsManager.
 */
class SdlGraphicsManager {
public:
	SdlGraphicsManager(SdlEventSource *source);
	virtual ~SdlGraphicsManager();

	/**
	 * Notify the graphics manager that the graphics needs to be redrawn, since
	 * the application window was modified.
	 *
	 * This is basically called when SDL_VIDEOEXPOSE was received.
	 */
	virtual void notifyVideoExpose() = 0;

	/**
	 * Notify the graphics manager about an resize event.
	 *
	 * It is noteworthy that the requested width/height should actually be set
	 * up as is and not changed by the graphics manager, since else it might
	 * lead to odd behavior for certain window managers.
	 *
	 * It is only required to overwrite this method in case you want a
	 * resizable window. The default implementation just does nothing.
	 *
	 * @param width Requested window width.
	 * @param height Requested window height.
	 */
	virtual void notifyResize(const uint width, const uint height) {}

	/**
	 * Transforms real screen coordinates into the current active screen
	 * coordinates (may be either game screen or overlay).
	 *
	 * @param point Mouse coordinates to transform.
	 */
	virtual void transformMouseCoordinates(Common::Point &point) = 0;

	/**
	 * Notifies the graphics manager about a position change according to the
	 * real screen coordinates.
	 *
	 * @param mouse Mouse position.
	 */
	virtual void notifyMousePos(Common::Point mouse) = 0;
	virtual void initEventObserver();

	virtual bool hasFeature(OSystem::Feature f);
	virtual void setFeatureState(OSystem::Feature f, bool enable);
	virtual bool getFeatureState(OSystem::Feature f);

	virtual void launcherInitSize(uint w, uint h);
	byte *setupScreen(int screenW, int screenH, bool fullscreen, bool accel3d);
	virtual int16 getHeight();
	virtual int16 getWidth();

public:
	virtual Graphics::Surface *lockScreen(){return NULL;};
	virtual void unlockScreen(){};
	virtual void updateScreen();

	virtual void showOverlay();
	virtual void hideOverlay();
	virtual Graphics::PixelFormat getOverlayFormat() const { return _overlayFormat; }
	virtual void clearOverlay();
	virtual void grabOverlay(OverlayColor *buf, int pitch);
	virtual void copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h);
	virtual int16 getOverlayHeight() { return _overlayHeight; }
	virtual int16 getOverlayWidth() { return _overlayWidth; }
	void closeOverlay();

	virtual bool showMouse(bool visible);
	virtual void warpMouse(int x, int y);
	void setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale = 1, const Graphics::PixelFormat *format = NULL) {}
	
#ifdef USE_OSD
	virtual void displayMessageOnOSD(const char *msg);
#endif

	// Override from Common::EventObserver
	bool notifyEvent(const Common::Event &event);

protected:
	SdlEventSource *_eventSource;
};

#endif
