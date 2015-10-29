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

/** @file lib3ds_vector.c
    Vector mathematics implementation */

#include "lib3ds_impl.h"
#include "math/vector3d.h"
#include <math.h>

void lib3ds_vector_make(Math::Vector3d &c, float x, float y, float z) {
	c = Math::Vector3d(x, y, z);
}


void lib3ds_vector_zero(Math::Vector3d &c) {
	for (int i = 0; i < 3; ++i) {
		c.setValue(i, 0.0f);
	}
}

void lib3ds_vector_zero(float c[4]) {
	for (int i = 0; i < 3; ++i) {
		c[i] = 0.0f;
	}
}

void lib3ds_vector_copy(float dst[3], const Math::Vector3d &src) {
	for (int i = 0; i < 3; ++i) {
		dst[i] = src.getValue(i);
	}
}

void lib3ds_vector_copy(Math::Vector3d &dst, const Math::Vector3d &src) {
	dst = src;
}

/*!
 * Compute cross product.
 *
 * \param c Result.
 * \param a First vector.
 * \param b Second vector.
 */
void lib3ds_vector_cross(Math::Vector3d &c, const Math::Vector3d &a, const Math::Vector3d &b) {
	c = Math::Vector3d::crossProduct(a, b);
}


/*!
 * Compute length of vector.
 *
 * Computes |c| = sqrt(x*x + y*y + z*z)
 *
 * \param c Vector to compute.
 *
 * \return Length of vector.
 */
float lib3ds_vector_length(Math::Vector3d &c) {
	return c.getMagnitude();
}


/*!
 * Normalize a vector.
 *
 * Scales a vector so that its length is 1.0.
 *
 * \param c Vector to normalize.
 */
static void lib3ds_vector_normalize(Math::Vector3d &c) {
	float *p = c.getData();

	float l = c.getMagnitude();
	if (fabs(l) < LIB3DS_EPSILON) {
		if ((p[0] >= p[1]) && (p[0] >= p[2])) {
			p[0] = 1.0f;
			p[1] = p[2] = 0.0f;
		} else if (p[1] >= p[2]) {
			p[1] = 1.0f;
			p[0] = p[2] = 0.0f;
		} else {
			p[2] = 1.0f;
			p[0] = p[1] = 0.0f;
		}
	} else {
		c.normalize();
	}
}


/*!
 * Compute a vector normal to two line segments.
 *
 * Computes the normal vector to the lines b-a and b-c.
 *
 * \param n Returned normal vector.
 * \param a Endpoint of first line.
 * \param b Base point of both lines.
 * \param c Endpoint of second line.
 */
void lib3ds_vector_normal(Math::Vector3d &n, const Math::Vector3d &a, const Math::Vector3d &b, const Math::Vector3d &c) {
	Math::Vector3d p = c - b;
	Math::Vector3d q = a - b;
	n = Math::Vector3d::crossProduct(p, q);
	n.normalize();
}


/*!
 * Multiply a point by a transformation matrix.
 *
 * Applies the given transformation matrix to the given point.  With some
 * transformation matrices, a vector may also be transformed.
 *
 * \param c Result.
 * \param m Transformation matrix.
 * \param a Input point.
 */
void lib3ds_vector_transform(Math::Vector3d &c, const Math::Matrix4 &m, const Math::Vector3d &a) {
	c.setValue(0, m.getValue(0, 0) * a.getValue(0) + m.getValue(1, 0) * a.getValue(1) + m.getValue(2, 0) * a.getValue(2) + m.getValue(3, 0));
	c.setValue(1, m.getValue(0, 1) * a.getValue(0) + m.getValue(1, 1) * a.getValue(1) + m.getValue(2, 1) * a.getValue(2) + m.getValue(3, 1));
	c.setValue(2, m.getValue(0, 2) * a.getValue(0) + m.getValue(1, 2) * a.getValue(1) + m.getValue(2, 2) * a.getValue(2) + m.getValue(3, 2));
}


/*!
 * c[i] = min(c[i], a[i]);
 *
 * Computes minimum values of x,y,z independently.
 */
void lib3ds_vector_min(Math::Vector3d &c, const Math::Vector3d &a) {
	for (int i = 0; i < 3; ++i) {
		if (a.getValue(i) < c.getValue(i)) {
			c.setValue(i, a.getValue(i));
		}
	}
}


/*!
 * c[i] = max(c[i], a[i]);
 *
 * Computes maximum values of x,y,z independently.
 */
void lib3ds_vector_max(Math::Vector3d &c, const Math::Vector3d &a) {
	for (int i = 0; i < 3; ++i) {
		if (a.getValue(i) > c.getValue(i)) {
			c.setValue(i, a.getValue(i));
		}
	}
}


