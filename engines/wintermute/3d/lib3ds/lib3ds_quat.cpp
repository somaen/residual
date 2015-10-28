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

/** @file lib3ds_quat.c
    Quaternion mathematics implementation */

#include "lib3ds_impl.h"
#include "math/quat.h"
#include <stdio.h>
#include <math.h>

/*!
 * Set a quaternion to Identity
 */
void lib3ds_quat_identity(Math::Quaternion &c) {
	c = Math::Quaternion();
}


/*!
 * Copy a quaternion.
 */
void lib3ds_quat_copy(Math::Quaternion &dest, const Math::Quaternion &src) {
	dest = src;
}

/*!
 * Copy a quaternion.
 */
void lib3ds_quat_copy(Math::Quaternion &dest, float src[4]) {
	dest = Math::Quaternion(src[0], src[1], src[2], src[3]);
}


/*!
 * Compute a quaternion from axis and angle.
 *
 * \param c Computed quaternion
 * \param axis Rotation axis
 * \param angle Angle of rotation, radians.
 */
void lib3ds_quat_axis_angle(Math::Quaternion &c, const Math::Vector3d &axis, float angle) {
	double omega, s;
	double l;

	l = axis.getMagnitude();
	if (l < LIB3DS_EPSILON) {
		c = Math::Quaternion();
	} else {
		omega = -0.5 * angle;
		s = sin(omega) / l;
		c.setValue(0, (float)s * axis.getValue(0));
		c.setValue(1, (float)s * axis.getValue(1));
		c.setValue(2, (float)s * axis.getValue(2));
		c.setValue(3, (float)cos(omega));
	}
}

void lib3ds_quat_axis_angle(Math::Quaternion &c, const Math::Quaternion &axis, float angle) {
	Math::Vector3d temp(axis.getValue(0), axis.getValue(1), axis.getValue(2));
	lib3ds_quat_axis_angle(c, temp, angle);
}


/*!
 * Negate a quaternion
 */
void lib3ds_quat_neg(Math::Quaternion &c) {
	c = c * -1;
}


/*!
 * Compute the conjugate of a quaternion
 */
void lib3ds_quat_cnj(Math::Quaternion &c) {
	for (int i = 0; i < 3; ++i) {
		c.setValue(i, -c.getValue(i));
	}
}


/*!
 * Multiply two quaternions.
 *
 * \param c Result
 * \param a,b Inputs
 */
void lib3ds_quat_mul(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b) {
	c = a * b;
}


/*!
 * Multiply a quaternion by a scalar.
 */
void lib3ds_quat_scalar(Math::Quaternion &c, float k) {
	c = c * k;
}


/*!
 * Normalize a quaternion.
 */
void lib3ds_quat_normalize(Math::Quaternion &c) {
	double l =  c.getMagnitude();
	if (fabs(l) < LIB3DS_EPSILON) {
		c = Math::Quaternion();
	} else {
		c.normalize();
	}
}


/*!
 * Compute the inverse of a quaternion.
 */
void lib3ds_quat_inv(Math::Quaternion &c) {
	double l =  c.getMagnitude();
	if (fabs(l) < LIB3DS_EPSILON) {
		c = Math::Quaternion();
	} else {
		c = c.inverse();
	}
}


/*!
 * Compute the dot-product of a quaternion.
 */
float lib3ds_quat_dot(const Math::Quaternion &a, const Math::Quaternion &b) {
	return a.dotProduct(b);
}


float lib3ds_quat_norm(Math::Quaternion &c) {
	return c.x() * c.x() + c.y() * c.y() + c.z() * c.z() + c.w() * c.w();
}


void lib3ds_quat_ln(Math::Quaternion &c) {
	double t;

	double s = sqrt(c.x() * c.x() + c.y() * c.y() + c.z() * c.z());
	double om = atan2(s, (double)c.w());
	if (fabs(s) < LIB3DS_EPSILON) {
		t = 0.0f;
	} else {
		t = om / s;
	}
	{
		c = c * t;
		c.w() = 0.0f;
	}
}


void lib3ds_quat_ln_dif(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b) {
	Math::Quaternion invp = a;;

	lib3ds_quat_inv(invp);
	lib3ds_quat_mul(c, invp, b);
	lib3ds_quat_ln(c);
}


void lib3ds_quat_exp(Math::Quaternion &c) {
	double om, sinom;

	om = sqrt(c.x() * c.x() + c.y() * c.y() + c.z() * c.z());
	if (fabs(om) < LIB3DS_EPSILON) {
		sinom = 1.0f;
	} else {
		sinom = sin(om) / om;
	}
	{
		c = c * sinom;
		c.w() = (float)cos(om);
	}
}


void lib3ds_quat_slerp(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &b, float t) {
	double sp, sq;
	float flip = 1.0f;

	double l = a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
	if (l < 0) {
		flip = -1.0f;
		l = -l;
	}

	double om = acos(l);
	double sinom = sin(om);
	if (fabs(sinom) > LIB3DS_EPSILON) {
		sp = sin((1.0f - t) * om) / sinom;
		sq = sin(t * om) / sinom;
	} else {
		sp = 1.0f - t;
		sq = t;
	}
	sq *= flip;
	for (int i = 0; i < 4; ++i) {
		c.setValue(i, (float)(sp * a.getValue(i) + sq * b.getValue(i)));
	}
}


void lib3ds_quat_squad(Math::Quaternion &c, const Math::Quaternion &a, const Math::Quaternion &p, const Math::Quaternion &q, const Math::Quaternion &b, float t) {
	Math::Quaternion ab, pq;

	lib3ds_quat_slerp(ab, a, b, t);
	lib3ds_quat_slerp(pq, p, q, t);
	lib3ds_quat_slerp(c, ab, pq, 2 * t * (1 - t));
}


void lib3ds_quat_tangent(Math::Quaternion &c, const Math::Quaternion &p, const Math::Quaternion &q, const Math::Quaternion &n) {
	Math::Quaternion dn, dp, x;

	lib3ds_quat_ln_dif(dn, q, n);
	lib3ds_quat_ln_dif(dp, q, p);

	for (int i = 0; i < 4; i++) {
		x.setValue(i, -1.0f / 4.0f * (dn.getValue(i) + dp.getValue(i)));
	}
	lib3ds_quat_exp(x);
	lib3ds_quat_mul(c, q, x);
}


void lib3ds_quat_dump(const Math::Quaternion &q) {
	printf("%f %f %f %f\n", q.x(), q.y(), q.z(), q.w());
}

