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

 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 */

#include "common/endian.h"
#include "common/system.h"

#include "graphics/surface.h"
/*
#include "engines/grim/actor.h"
#include "engines/grim/colormap.h"
#include "engines/grim/material.h"
#include "engines/grim/font.h"*/
#include "engines/fitd/gfx_tinygl.h"
#include "engines/fitd/fitd.h"
/*#include "engines/grim/lipsync.h"
#include "engines/grim/bitmap.h"
#include "engines/grim/primitives.h"
#include "engines/grim/model.h"
#include "engines/grim/set.h"*/

namespace Fitd {

GfxBase *CreateGfxTinyGL() {
	return new GfxTinyGL();
}

GfxTinyGL::GfxTinyGL() {
	g_driver = this;
	_zb = NULL;
	_storedDisplay = NULL;
}

GfxTinyGL::~GfxTinyGL() {
	delete[] _storedDisplay;
	if (_zb) {
		TinyGL::glClose();
		ZB_close(_zb);
	}
}

byte *GfxTinyGL::setupScreen(int screenW, int screenH, bool fullscreen) {
	byte *buffer = g_system->setupScreen(screenW, screenH, fullscreen, false);

	_screenWidth = screenW;
	_screenHeight = screenH;
	_screenBPP = 15;
	_isFullscreen = g_system->getFeatureState(OSystem::kFeatureFullscreenMode);

	g_system->showMouse(!fullscreen);

	g_system->setWindowCaption("Residual: Software 3D Renderer");

	_zb = TinyGL::ZB_open(screenW, screenH, ZB_MODE_5R6G5B, buffer);
	TinyGL::glInit(_zb);

	_storedDisplay = new byte[640 * 480 * 2];
	memset(_storedDisplay, 0, 640 * 480 * 2);

	_currentShadowArray = NULL;

	TGLfloat ambientSource[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	tglLightModelfv(TGL_LIGHT_MODEL_AMBIENT, ambientSource);

	return buffer;
}

const char *GfxTinyGL::getVideoDeviceName() {
	return "TinyGL Software Renderer";
}

void GfxTinyGL::setupCamera(float fov, float nclip, float fclip, float roll) {
	tglMatrixMode(TGL_PROJECTION);
	tglLoadIdentity();

	float right = nclip * tan(fov / 2 * (LOCAL_PI / 180));
	tglFrustum(-right, right, -right * 0.75, right * 0.75, nclip, fclip);

	tglMatrixMode(TGL_MODELVIEW);
	tglLoadIdentity();

	tglRotatef(roll, 0, 0, -1);
}

void GfxTinyGL::positionCamera(Math::Vector3d pos, Math::Vector3d interest) {
	Math::Vector3d up_vec(0, 0, 1);

	if (pos.x() == interest.x() && pos.y() == interest.y())
		up_vec = Math::Vector3d(0, 1, 0);

	lookAt(pos.x(), pos.y(), pos.z(), interest.x(), interest.y(), interest.z(), up_vec.x(), up_vec.y(), up_vec.z());
}

void GfxTinyGL::clearScreen() {
	memset(_zb->pbuf, 0, 640 * 480 * 2);
	memset(_zb->zbuf, 0, 640 * 480 * 2);
	memset(_zb->zbuf2, 0, 640 * 480 * 4);
}

void GfxTinyGL::flipBuffer() {
	g_system->updateScreen();
}

bool GfxTinyGL::isHardwareAccelerated() {
	return false;
}

} // end of namespace Grim
