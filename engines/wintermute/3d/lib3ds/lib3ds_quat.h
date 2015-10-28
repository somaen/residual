/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_QUAT_H
#define INCLUDED_LIB3DS_QUAT_H
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

/** @file lib3ds.h
    Header file for public API defined by lib3ds */
#include "common/stream.h"
#include "math/quat.h"
#include <stddef.h>

#ifndef LIB3DSAPI
#if defined(_MSC_VER) && !defined(LIB3DS_STATIC)
#ifdef LIB3DS_EXPORTS
#define LIB3DSAPI __declspec(dllexport)
#else
#define LIB3DSAPI __declspec(dllimport)
#endif
#else
#define LIB3DSAPI
#endif
#endif

extern LIB3DSAPI void lib3ds_quat_identity(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_copy(Math::Quaternion &dest, const Math::Quaternion &src);
extern LIB3DSAPI void lib3ds_quat_copy(Math::Quaternion &dest, float src[4]);
extern LIB3DSAPI void lib3ds_quat_axis_angle(Math::Quaternion &c, const Math::Quaternion &axis, float angle);
extern LIB3DSAPI void lib3ds_quat_axis_angle(Math::Quaternion &c, const Math::Vector3d &axis, float angle);
extern LIB3DSAPI void lib3ds_quat_neg(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_cnj(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_mul(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b);
extern LIB3DSAPI void lib3ds_quat_scalar(Math::Quaternion &c, float k);
extern LIB3DSAPI void lib3ds_quat_normalize(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_inv(Math::Quaternion &c);
extern LIB3DSAPI float lib3ds_quat_dot(const Math::Quaternion &a, const Math::Quaternion &b);
extern LIB3DSAPI float lib3ds_quat_norm(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_ln(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_ln_dif(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b);
extern LIB3DSAPI void lib3ds_quat_exp(Math::Quaternion &c);
extern LIB3DSAPI void lib3ds_quat_slerp(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b, float t);
extern LIB3DSAPI void lib3ds_quat_squad(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &p, const Math::Quaternion &q, const Math::Quaternion &b, float t);
extern LIB3DSAPI void lib3ds_quat_tangent(Math::Quaternion &c, float p[4], float q[4], float n[4]);

/** @} */
#endif
