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
#include "lib3ds_impl.h"
#include "lib3ds_io.h"
#include "lib3ds_vector.h"
#include "lib3ds_quat.h"
#include "lib3ds_math.h"
#include <assert.h>
#include <math.h>

template<> Lib3dsTrack<Lib3dsBoolKey>::Lib3dsTrack() : _type(LIB3DS_TRACK_BOOL) {}

template<> Lib3dsTrack<Lib3dsFloatKey>::Lib3dsTrack() : _type(LIB3DS_TRACK_FLOAT) {}

template<> Lib3dsTrack<Lib3dsVectorKey>::Lib3dsTrack() : _type(LIB3DS_TRACK_VECTOR) {}

template<> Lib3dsTrack<Lib3dsQuatKey>::Lib3dsTrack() : _type(LIB3DS_TRACK_QUAT) {}

template<typename T>
static void pos_key_setup(int n, Lib3dsKey<T> *pp, Lib3dsKey<T> *pc, Lib3dsKey<T> *pn, float *dd, float *ds) {
	float tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp, c;
	float dt, fp, fn;
	float delm[3], delp[3];

	assert(pc);
	fp = fn = 1.0f;
	if (pp && pn) {
		dt = 0.5f * (pn->frame - pp->frame);
		fp = (float)(pc->frame - pp->frame) / dt;
		fn = (float)(pn->frame - pc->frame) / dt;
		c  = (float)fabs(pc->cont);
		fp = fp + c - c * fp;
		fn = fn + c - c * fn;
	}

	cm = 1.0f - pc->cont;
	tm = 0.5f * (1.0f - pc->tens);
	cp = 2.0f - cm;
	bm = 1.0f - pc->bias;
	bp = 2.0f - bm;
	tmcm = tm * cm;
	tmcp = tm * cp;
	ksm = tmcm * bp * fp;
	ksp = tmcp * bm * fp;
	kdm = tmcp * bp * fn;
	kdp = tmcm * bm * fn;

	for (int i = 0; i < n; ++i) {
		delm[i] = delp[i] = 0;
	}
	if (pp) {
		for (int i = 0; i < n; ++i) {
			delm[i] = pc->value.getValue(i) - pp->value.getValue(i);
		}
	}
	if (pn) {
		for (int i = 0; i < n; ++i) {
			delp[i] = pn->value.getValue(i) - pc->value.getValue(i);
		}
	}
	if (!pp) {
		for (int i = 0; i < n; ++i) {
			delm[i] = delp[i];
		}
	}
	if (!pn) {
		for (int i = 0; i < n; ++i) {
			delp[i] = delm[i];
		}
	}

	for (int i = 0; i < n; ++i) {
		ds[i] = ksm * delm[i] + ksp * delp[i];
		dd[i] = kdm * delm[i] + kdp * delp[i];
	}
}

template<>
static void pos_key_setup(int n, Lib3dsKey<float> *pp, Lib3dsKey<float> *pc, Lib3dsKey<float> *pn, float *dd, float *ds) {
	float tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp, c;
	float dt, fp, fn;
	float delm[3], delp[3];

	assert(pc);
	fp = fn = 1.0f;
	if (pp && pn) {
		dt = 0.5f * (pn->frame - pp->frame);
		fp = (float)(pc->frame - pp->frame) / dt;
		fn = (float)(pn->frame - pc->frame) / dt;
		c  = (float)fabs(pc->cont);
		fp = fp + c - c * fp;
		fn = fn + c - c * fn;
	}

	cm = 1.0f - pc->cont;
	tm = 0.5f * (1.0f - pc->tens);
	cp = 2.0f - cm;
	bm = 1.0f - pc->bias;
	bp = 2.0f - bm;
	tmcm = tm * cm;
	tmcp = tm * cp;
	ksm = tmcm * bp * fp;
	ksp = tmcp * bm * fp;
	kdm = tmcp * bp * fn;
	kdp = tmcm * bm * fn;

	for (int i = 0; i < n; ++i) {
		delm[i] = delp[i] = 0;
	}
	if (pp) {
		for (int i = 0; i < n; ++i) {
			delm[i] = pc->value - pp->value;
		}
	}
	if (pn) {
		for (int i = 0; i < n; ++i) {
			delp[i] = pn->value - pc->value;
		}
	}
	if (!pp) {
		for (int i = 0; i < n; ++i) {
			delm[i] = delp[i];
		}
	}
	if (!pn) {
		for (int i = 0; i < n; ++i) {
			delp[i] = delm[i];
		}
	}

	for (int i = 0; i < n; ++i) {
		ds[i] = ksm * delm[i] + ksp * delp[i];
		dd[i] = kdm * delm[i] + kdp * delp[i];
	}
}

static void rot_key_setup(Lib3dsKey<Math::Quaternion> *prev, Lib3dsKey<Math::Quaternion> *cur, Lib3dsKey<Math::Quaternion> *next, Math::Quaternion &a, Math::Quaternion &b) {
	float tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp, c;
	float dt, fp, fn;
	Math::Quaternion q, qm, qp, qa, qb;
	int i;

	assert(cur);
	if (prev) {
		if (cur->value.getValue(3) > (2*M_PI) - LIB3DS_EPSILON) {
			lib3ds_quat_axis_angle(qm, cur->value, 0.0f);
			lib3ds_quat_ln(qm);
		} else {
			lib3ds_quat_copy(q, prev->value);
			if (lib3ds_quat_dot(q, cur->value) < 0)
				lib3ds_quat_neg(q);
			lib3ds_quat_ln_dif(qm, q, cur->value);
		}
	}
	if (next) {
		if (next->value.getValue(3) > (2*M_PI) - LIB3DS_EPSILON) {
			lib3ds_quat_axis_angle(qp, next->value, 0.0f);
			lib3ds_quat_ln(qp);
		} else {
			lib3ds_quat_copy(q, next->value);
			if (lib3ds_quat_dot(q, cur->value) < 0) lib3ds_quat_neg(q);
			lib3ds_quat_ln_dif(qp, cur->value, q);
		}
	}
	if (!prev) lib3ds_quat_copy(qm, qp);
	if (!next) lib3ds_quat_copy(qp, qm);

	fp = fn = 1.0f;
	cm = 1.0f - cur->cont;
	if (prev && next) {
		dt = 0.5f * (next->frame - prev->frame);
		fp = (float)(cur->frame - prev->frame) / dt;
		fn = (float)(next->frame - cur->frame) / dt;
		c  = (float)fabs(cur->cont);
		fp = fp + c - c * fp;
		fn = fn + c - c * fn;
	}

	tm = 0.5f * (1.0f - cur->tens);
	cp = 2.0f - cm;
	bm = 1.0f - cur->bias;
	bp = 2.0f - bm;
	tmcm = tm * cm;
	tmcp = tm * cp;
	ksm = 1.0f - tmcm * bp * fp;
	ksp = -tmcp * bm * fp;
	kdm = tmcp * bp * fn;
	kdp = tmcm * bm * fn - 1.0f;

	for (i = 0; i < 4; i++) {
		qa.setValue(i, 0.5f * (kdm * qm.getValue(i) + kdp * qp.getValue(i)));
		qb.setValue(i, 0.5f * (ksm * qm.getValue(i) + ksp * qp.getValue(i)));
	}
	lib3ds_quat_exp(qa);
	lib3ds_quat_exp(qb);

	lib3ds_quat_mul(a, cur->value, qa);
	lib3ds_quat_mul(b, cur->value, qb);
}


static void quat_for_index(Lib3dsQuatTrack *track, int index, Math::Quaternion &q) {
	Math::Quaternion p;
	lib3ds_quat_identity(q);
	for (int i = 0; i <= index; ++i) {
		lib3ds_quat_axis_angle(p, track->keys[i].value, track->keys[i].value.getValue(3));
		lib3ds_quat_mul(q, p, q);
	}
}

template<typename T>
static int find_index(Lib3dsTrack<T> *track, float t, float *u) {
	float nt;

	assert(track);
	assert(track->nkeys > 0);

	if (track->nkeys <= 1)
		return -1;

	int t0 = track->keys[0].frame;
	int t1 = track->keys[track->nkeys - 1].frame;
	if (track->flags & LIB3DS_TRACK_REPEAT) {
		nt = (float)fmod((float)(t - t0), (float)(t1 - t0)) + t0;
	} else {
		nt = t;
	}

	if (nt <= t0) {
		return -1;
	}
	if (nt >= t1) {
		return track->nkeys;
	}
	
	int i = 0;
	for (i = 1; i < track->nkeys; ++i) {
		if (nt < track->keys[i].frame)
			break;
	}

	*u = nt - (float)track->keys[i - 1].frame;
	*u /= (float)(track->keys[i].frame - track->keys[i - 1].frame);

	assert((*u >= 0.0f) && (*u <= 1.0f));
	return i;
}

template<typename T>
static void setup_segment(Lib3dsTrack<Lib3dsKey<T> > *track, int index, Lib3dsKey<T> *pp, Lib3dsKey<T> *p0, Lib3dsKey<T> *p1, Lib3dsKey<T> *pn) {
	int ip = 0;
	int in = 0;

	pp->frame = pn->frame = -1;
	if (index >= 2) {
		ip = index - 2;
		*pp = track->keys[index - 2];
	} else {
		if (track->flags & LIB3DS_TRACK_SMOOTH) {
			ip = track->nkeys - 2;
			*pp = track->keys[track->nkeys - 2];
			pp->frame = track->keys[track->nkeys - 2].frame - (track->keys[track->nkeys - 1].frame - track->keys[0].frame);
		}
	}

	*p0 = track->keys[index - 1];
	*p1 = track->keys[index];

	if (index < (int)track->nkeys - 1) {
		in = index + 1;
		*pn = track->keys[index + 1];
	} else {
		if (track->flags & LIB3DS_TRACK_SMOOTH) {
			in = 1;
			*pn = track->keys[1];
			pn->frame = track->keys[1].frame + (track->keys[track->nkeys - 1].frame - track->keys[0].frame);
		}
	}

	if (track->_type == LIB3DS_TRACK_QUAT) {
		assert(false);
	}
}

template<>
void setup_segment(Lib3dsTrack<Lib3dsKey<Math::Quaternion> > *track, int index, Lib3dsKey<Math::Quaternion> *pp, Lib3dsKey<Math::Quaternion> *p0, Lib3dsKey<Math::Quaternion> *p1, Lib3dsKey<Math::Quaternion> *pn) {
	int ip = 0;
	int in = 0;

	pp->frame = pn->frame = -1;
	if (index >= 2) {
		ip = index - 2;
		*pp = track->keys[index - 2];
	} else {
		if (track->flags & LIB3DS_TRACK_SMOOTH) {
			ip = track->nkeys - 2;
			*pp = track->keys[track->nkeys - 2];
			pp->frame = track->keys[track->nkeys - 2].frame - (track->keys[track->nkeys - 1].frame - track->keys[0].frame);
		}
	}

	*p0 = track->keys[index - 1];
	*p1 = track->keys[index];

	if (index < (int)track->nkeys - 1) {
		in = index + 1;
		*pn = track->keys[index + 1];
	} else {
		if (track->flags & LIB3DS_TRACK_SMOOTH) {
			in = 1;
			*pn = track->keys[1];
			pn->frame = track->keys[1].frame + (track->keys[track->nkeys - 1].frame - track->keys[0].frame);
		}
	}

	if (track->_type == LIB3DS_TRACK_QUAT) {
		Math::Quaternion q;
		if (pp->frame >= 0) {
			quat_for_index(track, ip, pp->value);
		} else {
			lib3ds_quat_identity(pp->value);
		}

		quat_for_index(track, index - 1, p0->value);
		lib3ds_quat_axis_angle(q, track->keys[index].value, track->keys[index].value.getValue(3));
		lib3ds_quat_mul(p1->value, q, p0->value);

		if (pn->frame >= 0) {
			lib3ds_quat_axis_angle(q, track->keys[in].value, track->keys[in].value.getValue(3));
			lib3ds_quat_mul(pn->value, q, p1->value);
		} else {
			lib3ds_quat_identity(pn->value);
		}
	}
}


void lib3ds_track_eval_bool(Lib3dsTrack<Lib3dsBoolKey> *track, int *b, float t) {
	*b = false;
	if (track) {
		int index;
		float u;

		assert(track->_type == LIB3DS_TRACK_BOOL);
		if (!track->nkeys) {
			return;
		}

		index = find_index(track, t, &u);
		if (index < 0) {
			*b = false;
			return;
		}
		if (index >= track->nkeys) {
			*b = !(track->nkeys & 1);
			return;
		}
		*b = !(index & 1);
	}
}

template<typename T>
static void track_eval_linear(Lib3dsTrack<Lib3dsKey<T> > *track, float *value, float t) {
	Lib3dsKey<T> pp, p0, p1, pn;
	float u;
	int index;
	float dsp[3], ddp[3], dsn[3], ddn[3];

	assert(track);
	if (!track->nkeys) {
		for (int i = 0; i < track->_type; ++i) {
			value[i] = 0.0f;
		}
		return;
	}

	index = find_index(track, t, &u);

	if (index < 0) {
		for (int i = 0; i < track->_type; ++i) {
			value[i] = track->keys[0].value.getValue(i);
		}
		return;
	}
	if (index >= track->nkeys) {
		for (int i = 0; i < track->_type; ++i) {
			value[i] = track->keys[track->nkeys - 1].value.getValue(i);
		}
		return;
	}

	setup_segment(track, index, &pp, &p0, &p1, &pn);

	pos_key_setup(track->_type, pp.frame >= 0 ? &pp : NULL, &p0, &p1, ddp, dsp);
	pos_key_setup(track->_type, &p0, &p1, pn.frame >= 0 ? &pn : NULL, ddn, dsn);

	lib3ds_math_cubic_interp(
	    value,
	    p0.value.getData(),
	    ddp,
	    dsn,
	    p1.value.getData(),
	    track->_type,
	    u
	);
}

static void track_eval_linear(Lib3dsTrack<Lib3dsKey<float> > *track, float *value, float t) {
	Lib3dsKey<float> pp, p0, p1, pn;
	float u;
	int index;
	float dsp[3], ddp[3], dsn[3], ddn[3];

	assert(track);
	if (!track->nkeys) {
		for (int i = 0; i < track->_type; ++i) {
			value[i] = 0.0f;
		}
		return;
	}

	index = find_index(track, t, &u);

	if (index < 0) {
		for (int i = 0; i < track->_type; ++i) {
			value[i] = track->keys[0].value;
		}
		return;
	}
	if (index >= track->nkeys) {
		for (int i = 0; i < track->_type; ++i) {
			value[i] = track->keys[track->nkeys - 1].value;
		}
		return;
	}

	setup_segment(track, index, &pp, &p0, &p1, &pn);

	pos_key_setup(track->_type, pp.frame >= 0 ? &pp : NULL, &p0, &p1, ddp, dsp);
	pos_key_setup(track->_type, &p0, &p1, pn.frame >= 0 ? &pn : NULL, ddn, dsn);

	lib3ds_math_cubic_interp(
	    value,
	    &p0.value,
	    ddp,
	    dsn,
	    &p1.value,
	    track->_type,
	    u
	);
}

void lib3ds_track_eval_float(Lib3dsTrack<Lib3dsFloatKey> *track, float *f, float t) {
	*f = 0;
	if (track) {
		assert(track->_type == LIB3DS_TRACK_FLOAT);
		track_eval_linear(track, f, t);
	}
}


void lib3ds_track_eval_vector(Lib3dsTrack<Lib3dsVectorKey> *track, Math::Vector3d &v, float t) {
	v = Math::Vector3d(0, 0, 0);
	if (track) {
		assert(track->_type == LIB3DS_TRACK_VECTOR);
		track_eval_linear(track, v.getData(), t);
	}
}


void lib3ds_track_eval_quat(Lib3dsTrack<Lib3dsQuatKey> *track, Math::Quaternion &q, float t) {
	lib3ds_quat_identity(q);
	if (track) {
		Lib3dsKey<Math::Quaternion> pp, p0, p1, pn;
		float u;
		int index;
		Math::Quaternion ap, bp, an, bn;

		assert(track->_type == LIB3DS_TRACK_QUAT);
		if (!track->nkeys) {
			return;
		}

		index = find_index(track, t, &u);
		if (index < 0) {
			lib3ds_quat_axis_angle(q, track->keys[0].value, track->keys[0].value.getValue(3));
			return;
		}
		if (index >= track->nkeys) {
			quat_for_index(track, track->nkeys - 1, q);
			return;
		}

		setup_segment(track, index, &pp, &p0, &p1, &pn);

		rot_key_setup(pp.frame >= 0 ? &pp : NULL, &p0, &p1, ap, bp);
		rot_key_setup(&p0, &p1, pn.frame >= 0 ? &pn : NULL, an, bn);

		lib3ds_quat_squad(q, p0.value, ap, bn, p1.value, u);
	}
}

template<typename T>
static void tcb_read(Lib3dsKey<T> *key, Lib3dsIo *io) {
	Common::SeekableReadStream *stream = io->stream;

	key->flags = stream->readUint16LE();
	if (key->flags & LIB3DS_KEY_USE_TENS) {
		key->tens = lib3ds_io_read_float(stream);
	}
	if (key->flags & LIB3DS_KEY_USE_CONT) {
		key->cont = lib3ds_io_read_float(stream);
	}
	if (key->flags & LIB3DS_KEY_USE_BIAS) {
		key->bias = lib3ds_io_read_float(stream);
	}
	if (key->flags & LIB3DS_KEY_USE_EASE_TO) {
		key->ease_to = lib3ds_io_read_float(stream);
	}
	if (key->flags & LIB3DS_KEY_USE_EASE_FROM) {
		key->ease_from = lib3ds_io_read_float(stream);
	}
}

void lib3ds_track_read(Lib3dsTrack<Lib3dsBoolKey> *track, Lib3dsIo *io) {
	Common::SeekableReadStream *stream = io->stream;

	track->flags = stream->readUint16LE();
	stream->readUint32LE();
	stream->readUint32LE();
	unsigned nkeys = stream->readSint32LE();
	track->resize(nkeys);
	
	for (int i = 0; i < nkeys; ++i) {
		track->keys[i].frame = stream->readSint32LE();
		tcb_read(&track->keys[i], io);
	}
}

void lib3ds_track_read(Lib3dsTrack<Lib3dsFloatKey> *track, Lib3dsIo *io) {
	Common::SeekableReadStream *stream = io->stream;

	track->flags = stream->readUint16LE();
	stream->readUint32LE();
	stream->readUint32LE();
	unsigned nkeys = stream->readSint32LE();
	track->resize(nkeys);
	
	for (int i = 0; i < nkeys; ++i) {
		track->keys[i].frame = stream->readSint32LE();
		tcb_read(&track->keys[i], io);
		track->keys[i].value = lib3ds_io_read_float(stream);
	}
}

void lib3ds_track_read(Lib3dsTrack<Lib3dsVectorKey> *track, Lib3dsIo *io) {
	Common::SeekableReadStream *stream = io->stream;
	
	track->flags = stream->readUint16LE();
	stream->readUint32LE();
	stream->readUint32LE();
	unsigned nkeys = stream->readSint32LE();
	track->resize(nkeys);

	for (int i = 0; i < nkeys; ++i) {
		track->keys[i].frame = stream->readSint32LE();
		tcb_read(&track->keys[i], io);
		Math::Vector3d tmp;
		lib3ds_io_read_vector(stream, tmp);
		lib3ds_vector_copy(track->keys[i].value, tmp);
	}
}

void lib3ds_track_read(Lib3dsTrack<Lib3dsQuatKey> *track, Lib3dsIo *io) {
	Common::SeekableReadStream *stream = io->stream;
	
	track->flags = stream->readUint16LE();
	stream->readUint32LE();
	stream->readUint32LE();
	unsigned nkeys = stream->readSint32LE();
	track->resize(nkeys);
	
	for (int i = 0; i < nkeys; ++i) {
		track->keys[i].frame = stream->readSint32LE();
		tcb_read(&track->keys[i], io);
		track->keys[i].value.setValue(3, lib3ds_io_read_float(stream));
		Math::Vector3d tmp;
		lib3ds_io_read_vector(stream, tmp);
		track->keys[i].value.setValue(0, tmp.getValue(0));
		track->keys[i].value.setValue(1, tmp.getValue(1));
		track->keys[i].value.setValue(2, tmp.getValue(2));
	}
}

