/* Residual - A 3D game interpreter
 *
 * Residual is the legal property of its developers, whose names
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
 */

#ifndef FITD_H
#define FITD_H

#include "common/random.h"
#include "engines/engine.h"
#include "gui/debugger.h"

namespace Fitd {

enum FitdGameType {
	GType_AITD1,
	GType_AITD2,
	GType_AITD3,
	GType_JITD
};

class FitdEngine : public Engine {
public:
	FitdEngine(OSystem *sys, uint32 gameFlags, FitdGameType gameType, Common::Platform platform, Common::Language language);
	~FitdEngine();
	virtual Common::Error run();
private:
	Common::RandomSource *_rnd;
};

}

#endif // FITD_H
