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

#include "common/scummsys.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/file.h"
#include "common/fs.h"
#include "fitd.h"

namespace Fitd {

FitdEngine::FitdEngine(OSystem *sys, uint32 gameFlags, FitdGameType gameType, Common::Platform platform, Common::Language language) : Engine(sys) {
	const Common::FSNode gameDataDir(ConfMan.get("path"));

	_rnd = new Common::RandomSource("fitd");

	debug("FitdEngine::FitdEngine");
}

FitdEngine::~FitdEngine() {
	debug("FitdEngine::~FitdEngine");

	// Dispose your resources here
	delete _rnd;

	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();
}

Common::Error FitdEngine::run() {

}

}
