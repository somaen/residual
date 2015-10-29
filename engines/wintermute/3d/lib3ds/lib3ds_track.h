/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_TRACK_H
#define INCLUDED_LIB3DS_TRACK_H
/*
    Copyright (C) 1996-2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
    All rights reserved.

    This program is free  software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Thisprogram  is  distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should  have received a copy of the GNU General Public License
    along with  this program; If not, see <http://www.gnu.org/licenses/>.
 */

enum Lib3dsKeyFlags {
	LIB3DS_KEY_USE_TENS         = 0x01,
	LIB3DS_KEY_USE_CONT         = 0x02,
	LIB3DS_KEY_USE_BIAS         = 0x04,
	LIB3DS_KEY_USE_EASE_TO      = 0x08,
	LIB3DS_KEY_USE_EASE_FROM    = 0x10
};

template<typename T>
struct Lib3dsKey {
	int         frame;
	unsigned    flags;
	float       tens;
	float       cont;
	float       bias;
	float       ease_to;
	float       ease_from;
	T           value;
};

typedef Lib3dsKey<bool> Lib3dsBoolKey;
typedef Lib3dsKey<float> Lib3dsFloatKey;
typedef Lib3dsKey<Math::Vector3d> Lib3dsVectorKey;
typedef Lib3dsKey<Math::Quaternion> Lib3dsQuatKey;

enum Lib3dsTrackType {
	LIB3DS_TRACK_BOOL   = 0,
	LIB3DS_TRACK_FLOAT  = 1,
	LIB3DS_TRACK_VECTOR = 3,
	LIB3DS_TRACK_QUAT   = 4
};

enum {
	LIB3DS_TRACK_REPEAT   = 0x0001,
	LIB3DS_TRACK_SMOOTH   = 0x0002,
	LIB3DS_TRACK_LOCK_X   = 0x0008,
	LIB3DS_TRACK_LOCK_Y   = 0x0010,
	LIB3DS_TRACK_LOCK_Z   = 0x0020,
	LIB3DS_TRACK_UNLINK_X = 0x0100,
	LIB3DS_TRACK_UNLINK_Y = 0x0200,
	LIB3DS_TRACK_UNLINK_Z = 0x0400
};

template<typename T>
struct Lib3dsTrack {
public:
	Lib3dsTrack();
	unsigned        flags;
	int             nkeys;
	void resize(int newSize) {
		keys.resize(newSize);
	}
	Common::Array<T> keys;
	Lib3dsTrackType _type;
};

template<> Lib3dsTrack<Lib3dsBoolKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsBoolKey> Lib3dsBoolTrack;

template<> Lib3dsTrack<Lib3dsFloatKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsFloatKey> Lib3dsFloatTrack;

template<> Lib3dsTrack<Lib3dsVectorKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsVectorKey> Lib3dsVectorTrack;

template<> Lib3dsTrack<Lib3dsQuatKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsQuatKey> Lib3dsQuatTrack;

void lib3ds_track_eval_bool(Lib3dsTrack<Lib3dsBoolKey> *track, int *b, float t);
void lib3ds_track_eval_float(Lib3dsTrack<Lib3dsFloatKey> *track, float *f, float t);
void lib3ds_track_eval_vector(Lib3dsTrack<Lib3dsVectorKey> *track, Math::Vector3d &v, float t);
void lib3ds_track_eval_quat(Lib3dsTrack<Lib3dsQuatKey> *track, Math::Quaternion &q, float t);

/** @} */
#endif
