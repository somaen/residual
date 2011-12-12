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
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/scummsys.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/system.h"
#include "engines/fitd/registry.h"
#include "engines/fitd/gfx_base.h"
#include "engines/fitd/fitd.h"
#include "engines/fitd/common.h"
#include "engines/fitd/thread_code.h"

namespace Fitd {

FitdEngine *g_fitd;

enumCVars AITD1KnownCVars[]=
{
	SAMPLE_PAGE,
	BODY_FLAMME,
	MAX_WEIGHT_LOADABLE,
	TEXTE_CREDITS,
	SAMPLE_TONNERRE,
	INTRO_DETECTIVE,
	INTRO_HERITIERE,
	WORLD_NUM_PERSO,
	CHOOSE_PERSO,
	SAMPLE_CHOC,
	SAMPLE_PLOUF,
	REVERSE_OBJECT,
	KILLED_SORCERER,
	LIGHT_OBJECT,
	FOG_FLAG,
	DEAD_PERSO,
	UNKNOWN_CVAR
};

enumCVars AITD2KnownCVars[]=
{
	SAMPLE_PAGE,
	BODY_FLAMME,
	MAX_WEIGHT_LOADABLE,
	SAMPLE_CHOC,
	DEAD_PERSO,
	JET_SARBACANE,
	TIR_CANON,
	JET_SCALPEL,
	POIVRE,
	DORTOIR,
	EXT_JACK,
	NUM_MATRICE_PROTECT_1,
	NUM_MATRICE_PROTECT_2,
	NUM_PERSO,
	TYPE_INVENTAIRE,
	PROLOGUE,
	POIGNARD,
	UNKNOWN_CVAR
};

GfxBase *g_driver = NULL;

FitdEngine::FitdEngine(OSystem *sys, uint32 gameFlags, FitdGameType gameType, Common::Platform platform, Common::Language language) : Engine(sys) {
	const Common::FSNode gameDataDir(ConfMan.get("path"));

	g_fitd = this;
	
	_rnd = new Common::RandomSource("fitd");

	debug("FitdEngine::FitdEngine");

	g_registry = new Registry();

	_gameType = gameType;
	_gameFlags = gameFlags;
	_gamePlatform = platform;
	_gameLanguage = language;

#ifdef USE_OPENGL
	_softRenderer = (tolower(g_registry->get("soft_renderer", "false")[0]) == 't');
#else
	_softRenderer = true;
#endif

	
	_showFps = (tolower(g_registry->get("show_fps", "false")[0]) == 't');

}

FitdEngine::~FitdEngine() {
	debug("FitdEngine::~FitdEngine");

	// Dispose your resources here
	delete _rnd;

	if (g_registry) {
		g_registry->save();
		delete g_registry;
		g_registry = NULL;
	}

	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();
}
int FitdEngine::getFrameNum() {
	return (g_system->getMillis() - _startTime) / _speed;
}

Common::Error FitdEngine::run() {
	debug("FitdEngine::run()");
	_startTime = g_system->getMillis();
	// For debugging-purposes, we won't want fullscreen, as gdb is quirky to use without a window
	bool fullscreen = false;
	//bool fullscreen = (tolower(g_registry->get("fullscreen", "false")[0]) == 't');

	if (!_softRenderer && !g_system->hasFeature(OSystem::kFeatureOpenGL)){
		warning("gfx backend doesn't support hardware rendering");
		_softRenderer=true;
	}

	//if (_softRenderer)
	//	g_driver = CreateGfxTinyGL();
/*#ifdef USE_OPENGL
	else*/
	//	g_driver = CreateGfxOpenGL();
//#endif
	switch (_gameType) {
		case GType_AITD1:
			_numCVars = 35;
			currentCVarTable = AITD1KnownCVars;
			break;
		default:
			_numCVars = 70;
			currentCVarTable = AITD2KnownCVars;
			break;
	}
	
	g_driver = CreateGfxOpenGL();
	
	g_driver->setupScreen(800, 600, fullscreen);
	g_driver->initBuffer(scaledScreen,800,600);
	startThreadTimer();
	sysInit();
	
	paletteFill(g_driver->_palette,0,0,0);
	
	preloadResource();
	
	// Init stuff
	
	// Game specifics
	switch(_gameType)
	{
		case GType_AITD1:
		{

			fadeIn(g_driver->_palette);
			if(!make3dTatou())
			{
				warning("Calling makeIntroScreens");
				makeIntroScreens();
			}
			break;
		}
		case GType_JITD:
		{
			startGame(16,1,1);
			break;
		}
		case GType_AITD2:
		{
			startGame(8,0,0);
			break;
		}
		case GType_AITD3:
		{
			startGame(0,12,1);
			startGame(0,0,1);
			break;
		}
		case GType_TIMEGATE:
		{
			startGame(0,0,1);
			break;
		}
		default:
			error("Unknown gametype\n");
			break;
	}

	while(1)
	{
		int startupMenuResult = processStartupMenu();
		
		switch(startupMenuResult)
		{
			case -1: // timeout
			{
				CVars[getCVarsIdx(CHOOSE_PERSO)] = rand()&1;
				/*  startGame(7,1,0);
				 
				 if(!make3dTatou())
				 {
				 if(!makeIntroScreens())
				 {
				 makeSlideshow();
				 }
				 } */
				
				break;
			}
			case 0: // new game
			{
				/*  if(protectionToBeDone)
				 {
				 makeProtection();
				 protectionToBeDone = 0;
				 }*/
				
				if(selectHero()!=-1)
				{
					readKeyboard();
					while(input2)
						readKeyboard();
					
					if(g_fitd->getGameType() == GType_AITD1)
					{
						CVars[getCVarsIdx(CHOOSE_PERSO)] = 0;
					}
					
					switch(g_fitd->getGameType())
					{
						case GType_JITD:
						{
							startGame(16,1,0);
							break;
						}
						case GType_AITD2:
						{
							startGame(8,7,1);
							break;
						}
						case GType_AITD3:
						{
							startGame(0,12,1);
							break;
						}
						case GType_AITD1:
						{
							startGame(7,1,0);
							
							/*  if(!protectionState)
							 {
							 freeAll();
							 exit(-1);
							 }
							 */
							readKeyboard();
							while(input2)
								readKeyboard();
							
							startGame(0,0,1);
							break;
						}
					}
					/*
					 if(giveUp == 0)
					 {
					 freeAll();
					 exit(-1);
					 }*/
				}
				
				break;
			}
			case 1: // continue
			{
				/*  if(protectionToBeDone)
				 {
				 makeProtection();
				 protectionToBeDone = 0;
				 }*/
				
				if(restoreSave(12,0))
				{
					/*  if(!protectionState)
					 {
					 freeAll();
					 exit(-1);
					 }*/
					
					//          updateShaking();
					
					mainVar1 = 2;
					
					setupCamera();
					
					mainLoop(1);
					
					//          freeScene();
					
					fadeOut(8,0);
					
					/*  if(giveUp == 0)
					 {
					 freeAll();
					 exit(-1);
					 } */
				}
				
				break;
			}
			case 2: // exit
			{
				freeAll();
				exit(-1);
				
				break;
			}
		}
	}
	
	//return(0);
	
	

	return Common::kNoError;
}
	

// Still needs refactor:
void startGame(int startupFloor, int startupRoom, int allowSystemMenu)
{
	warning("startGame");
	g_fitd->initEngine();
	initVars();
	
	loadFloor(startupFloor);
	
	currentCamera = -1;
	
	loadRoom(startupRoom);
	
	startGameVar1 = 0;
	mainVar1 = 2;
	
	setupCamera();
	
	mainLoop(allowSystemMenu);
	
	/*freeScene();
	 
	 fadeOut(8,0);*/
	warning("startGame ended");
}

void FitdEngine::initEngine()
{
	warning("initEngine");
	uint8* pObjectData;
	uint8* pObjectDataBackup;
	unsigned long int objectDataSize;
	Common::File* fHandle = new Common::File();
	int i;
	int choosePersoBackup;
	
	if(!fHandle->open("OBJETS.ITD"))
		theEnd(0,"OBJETS.ITD");
	
	objectDataSize= fHandle->size();
	fHandle->seek(0,SEEK_SET);
	
	pObjectDataBackup = pObjectData = (uint8*)malloc(objectDataSize);
	ASSERT(pObjectData);
	
	fHandle->read(pObjectData,objectDataSize);
	delete fHandle;
	
	maxObjects = READ_LE_UINT16(pObjectData);
	pObjectData+=2;
	
	if(g_fitd->getGameType() == GType_AITD1) {
		//objectTable = (objectStruct*)malloc(300*sizeof(objectStruct));
		objectTable = new objectStruct[300];
	}
	else {
		//objectTable = (objectStruct*)malloc(maxObjects*sizeof(objectStruct));
		objectTable = new objectStruct[maxObjects];
	}
	
	for(int i=0;i<maxObjects;i++){

		objectTable[i].ownerIdx = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].body = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].flags = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].field_6 = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].foundBody = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].foundName = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].flags2 = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].foundLife = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].x = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].y = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].z = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].alpha = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].beta = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].gamma = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].stage = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].room = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].lifeMode = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].life = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].field_24 = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].anim = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].frame = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].animType = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].animInfo = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].trackMode = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].trackNumber = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		objectTable[i].positionInTrack = READ_LE_UINT16(pObjectData);
		pObjectData+=2;
		
		if(g_fitd->getGameType() >= GType_JITD)
		{
			objectTable[i].mark = READ_LE_UINT16(pObjectData);
			pObjectData+=2;
		}
		objectTable[i].flags |= 0x20;
	}
	
	free(pObjectDataBackup);
	
	
	/* fHandle = fopen("objDump.txt","w+");
	 for(i=0;i<maxObjects;i++)
	 {
	 fprintf(fHandle,"Object %d:", i);
	 
	 fprintf(fHandle,"\t body:%03d",objectTable[i].field_2);
	 fprintf(fHandle,"\t anim:%03d",objectTable[i].anim);
	 fprintf(fHandle,"\t stage:%01d",objectTable[i].stage);
	 fprintf(fHandle,"\t room:%02d",objectTable[i].room);
	 fprintf(fHandle,"\t lifeMode: %01d",objectTable[i].lifeMode);
	 fprintf(fHandle,"\t life: %02d",objectTable[i].life);
	 fprintf(fHandle,"\t beta: %03X",objectTable[i].beta);
	 
	 fprintf(fHandle,"\n");
	 }
	 fclose(fHandle);
	 
	 fHandle = fopen("objNames.txt","w+");
	 for(i=0;i<maxObjects;i++)
	 {
	 fprintf(fHandle,"obj%03d ",i);
	 if(objectTable[i].foundName == -1)
	 {
	 fprintf(fHandle,"-1\n");
	 }
	 else
	 {
	 textEntryStruct* name = getTextFromIdx(objectTable[i].foundName);
	 if(name)
	 fprintf(fHandle,"%s\n",name->textPtr);
	 else
	 fprintf(fHandle,"?\n");
	 }
	 }
	 fclose(fHandle); */
	//
	
	vars = (short int*)loadFromItd("VARS.ITD");
	
	varSize = fileSize;
	
	if(g_fitd->getGameType() == GType_AITD1)
	{
		choosePersoBackup = CVars[getCVarsIdx(CHOOSE_PERSO)]; // backup hero selection
	}
	
	fHandle = new Common::File();
	if(!fHandle->open("DEFINES.ITD"))
	{
		theEnd(0,"DEFINES.ITD");
	}
	
	///////////////////////////////////////////////
	{
		fHandle->read(CVars,_numCVars*2);
		delete fHandle;
		
		for(i = 0; i <_numCVars; i++)
		{
			CVars[i] = ((CVars[i]&0xFF)<<8) | ((CVars[i]&0xFF00)>>8);
		}
	}
	//////////////////////////////////////////////
	
	if(g_fitd->getGameType() == GType_AITD1)
	{
		CVars[getCVarsIdx(CHOOSE_PERSO)] = choosePersoBackup;
	}
	
	listLife = HQR_InitRessource("LISTLIFE", 10000, 100);
	listTrack = HQR_InitRessource("LISTTRAK", 1000, 10); 
	
	// TODO: missing dos memory check here
	
	if(g_fitd->getGameType() == GType_AITD1)
	{
		listBody = HQR_InitRessource(listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]],100000, 50); // was calculated from free mem size
		listAnim = HQR_InitRessource(listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]],100000, 50); // was calculated from free mem size
	}
	else
	{
		listBody = HQR_InitRessource("LISTBODY",100000, 50); // was calculated from free mem size
		listAnim = HQR_InitRessource("LISTANIM",100000, 50); // was calculated from free mem size
		
		listMatrix = HQR_InitRessource("LISTMAT",16000,5);
	}
	
	
	for(i=0;i<NUM_MAX_ACTOR;i++)
	{
		actorTable[i].field_0 = -1;
	}
	
	if(g_fitd->getGameType() == GType_AITD1)
	{
		currentCameraTarget = CVars[getCVarsIdx(WORLD_NUM_PERSO)];
	}
	warning("InitEngine ended");
}

void initVarsSub1()
{
	for(int i=0;i<5;i++)
	{
		messageTable[i].string = NULL;
	}
}

void initVars()
{
	giveUp = 0;
	if(g_fitd->getGameType() == GType_AITD1)
	{
		inHand = -1;
		numObjInInventory = 0;
	}
	else
	{
		for(int i=0;i<2;i++)
		{
			numObjInInventoryTable[i] = 0;
			inHandTable[i] = -1;
		}
	}
	
	action = 0;
	
	genVar1 = genVar2;
	genVar3 = genVar4;
	
	genVar5 = 0;
	genVar6 = 0;
	
	soundVar2 = -1;
	genVar7 = -1;
	soundVar1 = -1;
	currentMusic = -1;
	nextMusic = -1;
	
	lightVar1 = 0;
	lightVar2 = 0;
	
	genVar9 = -1;
	currentCameraTarget = -1;
	
	statusScreenAllowed = 1;
	
	initVarsSub1();
}

void FitdEngine::sysInit()
{
	warning("SysInit");
	int i;
	
	/*#ifndef PCLIKE
	 unsigned long int ltime;
	 #else*/
	//time_t ltime;
	//#endif
	Common::File *fHandle;
	
	setupScreen();
	//setupInterrupt();
	//setupInterrupt2();
	//setupInterrupt3();
	
	//setupVideoMode();
	
	//time( &ltime );
	
	// TODO: Fix so we use OSystem-randoms.
	//srand(ltime);
	
	if(!initMusicDriver())
	{
		musicConfigured = 0;
		musicEnabled = 0;
	}
	
	// TODO: reverse sound init code
	
	
	aux = (char*)malloc(65068);
	if(!aux)
	{
		theEnd(1,"Aux");
	}
	
	aux2 = (char*)malloc(64000);
	if(!aux2)
	{
		theEnd(1,"Aux2");
	}
	
	sysInitSub1(aux2,screen);
	/*  sysInitSub2(aux2);
	 sysInitSub3(aux2); */
	
	bufferAnim = (char*)malloc(4960);
	if(!bufferAnim)
	{
		theEnd(1,"BufferAnim");
	}
	
	CVars = (short int*)malloc(_numCVars * sizeof(short int));
	
	switch(g_fitd->getGameType())
	{
		case GType_JITD:
		case GType_AITD2:
		case GType_AITD3:
		case GType_TIMEGATE:
		{
			fontData = loadPakSafe("ITD_RESS",1);
			break;
		}
		case GType_AITD1:
		{
			fontData = loadPakSafe("ITD_RESS",5);
			break;
		}
	}
	
	initFont(fontData, 14);
	
	if(g_fitd->getGameType() == GType_AITD1)
	{
		initFont2(2,0);
	}
	else
	{
		initFont2(2,1);
	}
	
	switch(g_fitd->getGameType())
	{
		case GType_JITD:
		case GType_AITD2:
		case GType_AITD3:
		{
			aitdBoxGfx = loadPakSafe("ITD_RESS",0);
			break;
		}
		case GType_AITD1:
		{
			aitdBoxGfx = loadPakSafe("ITD_RESS",4);
			break;
		}
	}
	
	priority = loadFromItd("PRIORITY.ITD");
	
	fHandle = new Common::File();
	if(!fHandle->open("DEFINES.ITD"))
	{
		theEnd(0,"DEFINES.ITD");
	}
	
	///////////////////////////////////////////////
	{
		fHandle->read(CVars,_numCVars*2);
		//	fread(CVars,numCVars,2,fHandle);
		//fclose(fHandle);
		delete fHandle;
		for(int i=0;i<_numCVars;i++)
		{
			CVars[i] = ((CVars[i]&0xFF)<<8) | ((CVars[i]&0xFF00)>>8);
		}
	}
	//////////////////////////////////////////////
	
	allocTextes();
	
	//  if(musicConfigured)
	{
		listMus = HQR_InitRessource("ListMus",110000,40);
	}
	
	listSamp = HQR_InitRessource("ListSamp",64000,30);
	
	hqrUnk = HQR_Init(10000,50);
	warning("SysInit ended");
}

void sysInitSub1(char* var0, char* var1)
{
	screenSm1 = var0;
	screenSm2 = var0;
	
	screenSm3 = var1;
	screenSm4 = var1;
	screenSm5 = var1;
}

void preloadResource()
{
	char localPalette[768];
	
	if(g_fitd->getGameType() == GType_AITD2)
	{
		loadPakToPtr("ITD_RESS",59,aux);
	}
	else
	{
		loadPakToPtr("ITD_RESS",3,aux);
	}
	copyPalette(aux,g_driver->_palette);
	
	copyPalette(g_driver->_palette,localPalette);
	//  fadeInSub1(localPalette);
	
	// to finish
}

int makeIntroScreens()
{
	warning("makeIntroScreens");
	char* data;
	unsigned int chrono;
	
	data = loadPak("ITD_RESS",13);
	copyToScreen(data+770,unkScreenVar);
	make3dTatouUnk1(8,0);
	memcpy(screen,unkScreenVar,320*200);
	flipScreen();
	free(data);
	loadPakToPtr("ITD_RESS",7,aux);
	startChrono(&chrono);
	
	do
	{
		int time;
		
		process_events();
		readKeyboard();
		
		time = evalChrono(&chrono);
		warning("time: %d", time);
		if(time>=0x30)
			break;
		
	} while(input2 == 0 && input1 == 0);
	
	playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
	/*  soundVar2 = -1;
	 soundVar1 = -1;
	 soundVar2 = -1;
	 soundVar1 = 0; */
	//  readVar = 1;
	printText(CVars[getCVarsIdx(TEXTE_CREDITS)]+1,48, 2,260,197,1,26);
	warning("makeIntroScreens ended");
	return(0);
}
	
} // End of namespace Fitd
