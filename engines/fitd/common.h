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

#ifndef _COMMON_H_
#define _COMMON_H_

#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/endian.h"

#define NUM_MAX_CAMERA_IN_ROOM 20

#define HAS_YM3812 0

#ifndef USE_GL
#define USE_GL
#endif

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;

typedef signed char s8;
typedef signed short int s16;
typedef signed long int s32;

#include <stdlib.h>
#include <stdio.h>
//#include <time.h>
#include <string.h>
#ifdef WIN32
#include <search.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <assert.h>
#endif

#ifdef _DEBUG
#define ASSERT(exp) assert(exp)
#else
#define ASSERT(exp)
#endif

#ifdef _DEBUG
#define ASSERT_PTR(exp) assert(exp)
#else
#define ASSERT_PTR(exp)
#endif

#ifdef USE_GL
//#include <GL/gl.h>      // Header File For The OpenGL32 Library
//#include <GL/glu.h>     // Header File For The GLu32 Library
//#include <gl\glaux.h>   // Header File For The Glaux Library
#endif

//#include "SDL.h"

//////////////// GAME SPECIFIC DEFINES

//#define NUM_MAX_OBJ         300
#define NUM_MAX_ACTOR       50
#define NUM_MAX_TEXT        40
#define NUM_MAX_MESSAGE     5
#define INVENTORY_SIZE      30


// 250
#define NUM_MAX_TEXT_ENTRY  1000

//////////////////

enum enumCVars
{
  SAMPLE_PAGE = 0,
  BODY_FLAMME = 1,
  MAX_WEIGHT_LOADABLE = 2,
	TEXTE_CREDITS = 3,
	SAMPLE_TONNERRE = 4,
	INTRO_DETECTIVE = 5,
	INTRO_HERITIERE = 6,
	WORLD_NUM_PERSO = 7,
	CHOOSE_PERSO = 8,
	SAMPLE_CHOC = 9,
	SAMPLE_PLOUF = 10,
	REVERSE_OBJECT = 11,
	KILLED_SORCERER = 12,
	LIGHT_OBJECT = 13,
	FOG_FLAG = 14,
	DEAD_PERSO = 15,
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
  MATRICE_FORME,
  MATRICE_COULEUR,

  UNKNOWN_CVAR = -1 // for table padding, shouldn't be called !
};

//typedef enum enumCVars enumCVars;

//extern enumCVars AITD1KnownCVars[];
//extern enumCVars AITD2KnownCVars[];

extern enumCVars* currentCVarTable;

int getCVarsIdx(enumCVars);

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;
typedef signed char S8;
typedef signed short S16;
typedef signed long S32;

#define TYPE_MASK 0x1D1

#define ANIM_ONCE             0
#define ANIM_REPEAT           1
#define ANIM_UNINTERRUPTABLE  2
#define ANIM_RESET            4

// temp triangulation stuff
//#include <bool.h>
//#include <geometry.h>
int triangulate_polygon(int ncontours,int cntr[],double (*vertices)[2],int (*triangles)[3]);
#include "common/scummsys.h"
#include "room.h"
#include "vars.h"
#include "main.h"
#include "file_access.h"
#include "screen.h"
#include "video_mode.h"
#include "pak.h"
#include "unpack.h"
#include "tatou.h"
#include "thread_code.h"
#include "renderer.h"
#include "input.h"
#include "version.h"
#include "cos_table.h"
#include "hqr.h"
#include "time.h"
#include "font.h"
#include "aitd_box.h"
#include "save.h"
#include "anim.h"
#include "anim_action.h"
#include "actor_list.h"
#include "main_loop.h"
#include "inventory.h"
#include "startup_menu.h"
#include "system_menu.h"
#include "floor.h"
#include "object.h"
#include "zv.h"
#include "music.h"
//#include "fmopl.h"

// debugger
#ifdef INTERNAL_DEBUGGER
#include "debugger.h"
#endif

// scripting
#include "track.h"
#include "life.h"
#include "life_2.h"
#include "eval_var.h"

#include "osystem.h"


////
/*
#ifdef UNIX
#define FORCEINLINE static inline
#else
#ifdef WIN32
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif
#endif
*/

#define READ_LE_U16(x) READ_LE_UINT16(x)
#define READ_LE_U32(x) READ_LE_UINT32(x)

FORCEINLINE s16 READ_LE_S16(void *ptr)
{
  return (s16)READ_LE_U16(ptr);
}


/*
FORCEINLINE s16 READ_BE_S16(void *ptr)
{
  return (s16)READ_BE_S16(ptr);
}*/


FORCEINLINE s32 READ_LE_S32(void *ptr)
{
  return (s32)READ_LE_U32(ptr);
}
/*
FORCEINLINE s32 READ_BE_S32(void *ptr)
{
  return (s32)READ_LE_U32(ptr);
}*/

#endif
