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

/** @file lib3ds_matrix.c
    Matrix mathematics implementation */

#include "lib3ds_impl.h"
#include "lib3ds_matrix.h"
#include "lib3ds_vector.h"
#include "lib3ds_quat.h"
#include "math/vector3d.h"
#include <string.h>
#include <math.h>

/*!
 * Clear a matrix to all zeros.
 *
 * \param m Matrix to be cleared.
 */
void lib3ds_matrix_zero(Math::Matrix4 &m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m.setValue(i, j, 0.0f);
		}
	}
}


/*!
 * Set a matrix to identity.
 *
 * \param m Matrix to be set.
 */
void lib3ds_matrix_identity(Math::Matrix4 &m) {
	lib3ds_matrix_zero(m);
	for (int i = 0; i < 4; i++) {
		m.setValue(i, i, 1.0);
	}
}


static float det2x2(float a, float b,
					  float c, float d) {
	return ((a) * (d) - (b) * (c));
}


static float det3x3(float a1, float a2, float a3,
					  float b1, float b2, float b3,
					  float c1, float c2, float c3) {
	return (
	           a1 * det2x2(b2, b3, c2, c3) -
	           b1 * det2x2(a2, a3, c2, c3) +
	           c1 * det2x2(a2, a3, b2, b3)
	       );
}


/*!
 * Find determinant of a matrix.
 */
float lib3ds_matrix_det(Math::Matrix4 &m) {
	float a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

	a1 = m.getValue(0, 0);
	b1 = m.getValue(1, 0);
	c1 = m.getValue(2, 0);
	d1 = m.getValue(3, 0);
	a2 = m.getValue(0, 1);
	b2 = m.getValue(1, 1);
	c2 = m.getValue(2, 1);
	d2 = m.getValue(3, 1);
	a3 = m.getValue(0, 2);
	b3 = m.getValue(1, 2);
	c3 = m.getValue(2, 2);
	d3 = m.getValue(3, 2);
	a4 = m.getValue(0, 3);
	b4 = m.getValue(1, 3);
	c4 = m.getValue(2, 3);
	d4 = m.getValue(3, 3);
	return (
	           a1 * det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4) -
	           b1 * det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4) +
	           c1 * det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4) -
	           d1 * det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4)
	       );
}


/*!
 * Apply a translation to a matrix.
 */
void lib3ds_matrix_translate(Math::Matrix4 &m, float x, float y, float z) {
	Math::Vector3d vec(x, y, z);
	m.translate(vec);
}


/*!
 * Apply scale factors to a matrix.
 */
void lib3ds_matrix_scale(Math::Matrix4 &m, float x, float y, float z) {
	for (int i = 0; i < 4; i++) {
		m.setValue(0, i, m.getValue(0, i) * x);
		m.setValue(1, i, m.getValue(1, i) * y);
		m.setValue(2, i, m.getValue(2, i) * z);
	}
}


/*!
 * Apply a rotation about an arbitrary axis to a matrix.
 */
void lib3ds_matrix_rotate_quat(Math::Matrix4 &m, float q[4]) {
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz, l;
	Math::Matrix4 R;

	l = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	if (fabs(l) < LIB3DS_EPSILON) {
		s = 1.0f;
	} else {
		s = 2.0f / l;
	}

	xs = q[0] * s;
	ys = q[1] * s;
	zs = q[2] * s;
	wx = q[3] * xs;
	wy = q[3] * ys;
	wz = q[3] * zs;
	xx = q[0] * xs;
	xy = q[0] * ys;
	xz = q[0] * zs;
	yy = q[1] * ys;
	yz = q[1] * zs;
	zz = q[2] * zs;

	R.setValue(0, 0, 1.0f - (yy + zz));
	R.setValue(1, 0, xy - wz);
	R.setValue(2, 0, xz + wy);
	R.setValue(0, 1, xy + wz);
	R.setValue(1, 1, 1.0f - (xx + zz));
	R.setValue(2, 1, yz - wx);
	R.setValue(0, 2, xz - wy);
	R.setValue(1, 2, yz + wx);
	R.setValue(2, 2, 1.0f - (xx + yy));
	R.setValue(3, 0, 0.0f);
	R.setValue(3, 1, 0.0f);
	R.setValue(3, 2, 0.0f);
	R.setValue(0, 3, 0.0f);
	R.setValue(1, 3, 0.0f);
	R.setValue(2, 3, 0.0f);
	R.setValue(3, 3, 1.0f);

	m = m * R;
}


/*!
 * Apply a rotation about an arbitrary axis to a matrix.
 */
void lib3ds_matrix_rotate(Math::Matrix4 &m, float angle, float ax, float ay, float az) {
	float q[4];

	Math::Vector3d axis(ax, ay, az);
	lib3ds_quat_axis_angle(q, axis, angle);
	lib3ds_matrix_rotate_quat(m, q);
}


/*!
 * Compute a camera matrix based on position, target and roll.
 *
 * Generates a translate/rotate matrix that maps world coordinates
 * to camera coordinates.  Resulting matrix does not include perspective
 * transform.
 *
 * \param matrix Destination matrix.
 * \param pos Camera position
 * \param tgt Camera target
 * \param roll Roll angle
 */
void lib3ds_matrix_camera(Math::Matrix4 &matrix, const Math::Vector3d &pos, const Math::Vector3d &tgt, float roll) {
	Math::Matrix4 M;

	Math::Vector3d y = tgt - pos;
	y.normalize();

	Math::Vector3d z;

	if (y.getValue(0) != 0. || y.getValue(1) != 0) {
		z = Math::Vector3d(0, 0, 1);
	} else { /* Special case:  looking straight up or down z axis */
		z = Math::Vector3d(-1.0, 0, 0);
	}

	Math::Vector3d x = Math::Vector3d::crossProduct(y, z);
	z = Math::Vector3d::crossProduct(x, y);
	x.normalize();
	z.normalize();

	lib3ds_matrix_identity(M);
	M.setValue(0, 0, x.getValue(0));
	M.setValue(1, 0, x.getValue(1));
	M.setValue(2, 0, x.getValue(2));
	M.setValue(0, 1, y.getValue(0));
	M.setValue(1, 1, y.getValue(1));
	M.setValue(2, 1, y.getValue(2));
	M.setValue(0, 2, z.getValue(0));
	M.setValue(1, 2, z.getValue(1));
	M.setValue(2, 2, z.getValue(2));

	lib3ds_matrix_identity(matrix);
	lib3ds_matrix_rotate(matrix, roll, 0, 1, 0);
	matrix = matrix * M;
	lib3ds_matrix_translate(matrix, -pos.getValue(0), -pos.getValue(1), -pos.getValue(2));
}
