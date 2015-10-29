/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_VECTOR_H
#define INCLUDED_LIB3DS_VECTOR_H
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

#include "common/stream.h"
#include <stddef.h>


/**
    Sets all components of a vector to zero.
    \param c
        The Pointer to the vector.
*/
void lib3ds_vector_zero(float c[4]);
void lib3ds_vector_zero(Math::Vector3d &c);

/**
    Copies all components of a vector to another vector.
    \param dst
        [out] The destination vector.
    \param src
        [in] The source vector.
*/
void lib3ds_vector_copy(float dst[4], const Math::Vector3d &src);
void lib3ds_vector_copy(Math::Vector3d &dst, const Math::Vector3d &src);

/**
    Negates all components of a vector.
    \param c
        The Pointer to the vector.
*/
void lib3ds_vector_neg(Math::Vector3d &c);
void lib3ds_vector_cross(Math::Vector3d &c, const Math::Vector3d &a, const Math::Vector3d &b);
float lib3ds_vector_length(Math::Vector3d &c);
void lib3ds_vector_normal(Math::Vector3d &n, const Math::Vector3d &a, const Math::Vector3d &b, const Math::Vector3d &c);
void lib3ds_vector_min(Math::Vector3d &c, const Math::Vector3d &a);
void lib3ds_vector_max(Math::Vector3d &c, const Math::Vector3d &a);
void lib3ds_vector_transform(Math::Vector3d &c, const Math::Matrix4 &m, const Math::Vector3d &a);

/** @} */
#endif
