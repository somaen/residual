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

#include "engines/advancedDetector.h"
#include "engines/obsolete.h"

#include "engines/fitd/fitd.h"
//#include "engines/grim/colormap.h"
//#include "engines/grim/savegame.h"

#include "common/system.h"
#include "common/savefile.h"

namespace Fitd {

struct FitdGameDescription {
	ADGameDescription desc;
	FitdGameType gameType;
};

static const PlainGameDescriptor aitdGames[] = {
	{"aitd", "Alone in the Dark"},
	{"fitd", "Free in the Dark-generic"},
	{"aitd2", "Alone in the Dark 2"},
	{"aitd3", "Alone in the Dark 3"},
	{"jitd", "Jack in the Dark"},
	{0, 0}
};

using Common::GUIO_NONE;

static const FitdGameDescription gameDescriptions[] = {
	{
		// Alone in the Dark - GOG.com-version
		{
			"aitd1",
			"",
			AD_ENTRY1s("CAMERA00.PAK", "e73cad1427181d79d758fd7ac0040a08", 143007),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO_NONE
		},
		GType_AITD1
	},
	{ AD_TABLE_END_MARKER, GType_AITD1 }
};

static const Engines::ObsoleteGameID obsoleteGameIDsTable[] = {
	{"aitd1", "aitd1", Common::kPlatformWindows},
	{0, 0, Common::kPlatformUnknown}
};

class FitdMetaEngine : public AdvancedMetaEngine {
public:
	FitdMetaEngine() : AdvancedMetaEngine(Fitd::gameDescriptions, sizeof(Fitd::FitdGameDescription), aitdGames) {
		_singleid = "fitd";
		_guioptions = Common::GUIO_NOMIDI;
	}

	virtual GameDescriptor findGame(const char *gameid) const {
		return Engines::findGameID(gameid, _gameids, obsoleteGameIDsTable);
	}

	virtual const char *getName() const {
		return "Free in the Dark Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "Alone in the Dark games (C) Infogrames";
	}

	virtual Common::Error createInstance(OSystem *syst, Engine **engine) const {
		Engines::upgradeTargetIfNecessary(obsoleteGameIDsTable);
		return AdvancedMetaEngine::createInstance(syst, engine);
	}

	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;

	virtual bool hasFeature(MetaEngineFeature f) const;

	virtual SaveStateList listSaves(const char *target) const;
};

bool FitdMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const FitdGameDescription *gd = (const FitdGameDescription *)desc;

	if (gd)
		*engine = new FitdEngine(syst, gd->desc.flags, gd->gameType, gd->desc.platform, gd->desc.language);

	return gd != 0;
}

bool FitdMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSupportsLoadingDuringStartup);
}

SaveStateList FitdMetaEngine::listSaves(const char *target) const {
/*	Common::SaveFileManager *saveFileMan = g_system->getSavefileManager();
	Common::StringArray filenames;
	Common::String saveDesc;
	Common::String pattern = "grim??.gsv";

	filenames = saveFileMan->listSavefiles(pattern);
	sort(filenames.begin(), filenames.end());	// Sort (hopefully ensuring we are sorted numerically..)
*/
	SaveStateList saveList;
/*	char str[256];
	int32 strSize;
	for (Common::StringArray::const_iterator file = filenames.begin(); file != filenames.end(); ++file) {
		// Obtain the last 2 digits of the filename, since they correspond to the save slot
		int slotNum = atoi(file->c_str() + 4);

		if (slotNum >= 0 && slotNum <= 99) {
			SaveGame *savedState = SaveGame::openForLoading((*file).c_str());
			if (savedState) {
				if (savedState->saveVersion() != SaveGame::SAVEGAME_VERSION) {
					delete savedState;
					continue;
				}
				savedState->beginSection('SUBS');
				strSize = savedState->readLESint32();
				savedState->read(str, strSize);
				saveDesc = str;
				saveList.push_back(SaveStateDescriptor(slotNum, saveDesc));
				delete savedState;
			}

		}
	}
*/
	return saveList;
}

} // End of namespace Grim

#if PLUGIN_ENABLED_DYNAMIC(FITD)
	REGISTER_PLUGIN_DYNAMIC(FITD, PLUGIN_TYPE_ENGINE, Fitd::FitdMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(FITD, PLUGIN_TYPE_ENGINE, Fitd::FitdMetaEngine);
#endif
