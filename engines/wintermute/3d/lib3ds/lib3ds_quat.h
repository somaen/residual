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

#include "math/quat.h"

void lib3ds_quat_identity(Math::Quaternion &c);
void lib3ds_quat_copy(Math::Quaternion &dest, const Math::Quaternion &src);
void lib3ds_quat_copy(Math::Quaternion &dest, float src[4]);
void lib3ds_quat_axis_angle(Math::Quaternion &c, const Math::Quaternion &axis, float angle);
void lib3ds_quat_axis_angle(Math::Quaternion &c, const Math::Vector3d &axis, float angle);
void lib3ds_quat_neg(Math::Quaternion &c);
void lib3ds_quat_cnj(Math::Quaternion &c);
void lib3ds_quat_mul(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b);
void lib3ds_quat_scalar(Math::Quaternion &c, float k);
void lib3ds_quat_normalize(Math::Quaternion &c);
void lib3ds_quat_inv(Math::Quaternion &c);
float lib3ds_quat_dot(const Math::Quaternion &a, const Math::Quaternion &b);
float lib3ds_quat_norm(Math::Quaternion &c);
void lib3ds_quat_ln(Math::Quaternion &c);
void lib3ds_quat_ln_dif(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b);
void lib3ds_quat_exp(Math::Quaternion &c);
void lib3ds_quat_slerp(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b, float t);
void lib3ds_quat_squad(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &p, const Math::Quaternion &q, const Math::Quaternion &b, float t);
void lib3ds_quat_tangent(Math::Quaternion &c, float p[4], float q[4], float n[4]);

/** @} */
#endif
