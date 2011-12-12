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
#include "engines/advancedDetector.h"
#include "engines/engine.h"
#include "gui/debugger.h"
#include "engines/fitd/startup_menu.h"

namespace Fitd {

enum FitdGameType {
	GType_AITD1,
	GType_JITD,
	GType_AITD2,
	GType_AITD3,
	GType_TIMEGATE
};

class FitdEngine : public Engine {
public:
	FitdEngine(OSystem *sys, uint32 gameFlags, FitdGameType gameType, Common::Platform platform, Common::Language language);
	~FitdEngine();
	FitdGameType getGameType() { return _gameType; }
	virtual Common::Error run();
	uint32 getRandom() { return 42; } // TODO
	int getFrameNum();
	void initEngine();
	void sysInit();
	int getNumCVars() { return _numCVars; }
private:
	Common::RandomSource *_rnd;

	int _numCVars;
	bool _showFps;
	bool _softRenderer;
	static const int _speed = 20;
	uint32 _startTime;
	uint32 _gameFlags;
	FitdGameType _gameType;
	Common::Platform _gamePlatform;
	Common::Language _gameLanguage;
};
	// Temporaries from main.cpp
	int makeIntroScreens();
	void preloadResource();
	//	void sysInit();
	void initVars();
	void initVarsSub1();
	//	void initEngine();
	void startGame(int startupFloor, int startupRoom, int allowSystemMenu);
	void sysInitSub1(char* var0, char* var1);

extern FitdEngine *g_fitd;
}
void allocTextes(void);

int selectHero();

#endif // FITD_H
