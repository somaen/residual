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

/** @file lib3ds.h
    Header file for public API defined by lib3ds */
#include "common/stream.h"
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

extern LIB3DSAPI void lib3ds_vector_make(
    float c[3],
    float x,
    float y,
    float z);

/**
    Sets all components of a vector to zero.
    \param c
        The Pointer to the vector.
*/
extern LIB3DSAPI void lib3ds_vector_zero(
    float c[3]);

/**
    Copies all components of a vector to another vector.
    \param dst
        [out] The destination vector.
    \param src
        [in] The source vector.
*/
extern LIB3DSAPI void lib3ds_vector_copy(
    float dst[3],
    float src[3]);

/**
    Negates all components of a vector.
    \param c
        The Pointer to the vector.
*/
extern LIB3DSAPI void lib3ds_vector_neg(
    float c[3]);

extern LIB3DSAPI void lib3ds_vector_make(float c[3], float x, float y, float z);
extern LIB3DSAPI void lib3ds_vector_zero(float c[3]);
extern LIB3DSAPI void lib3ds_vector_add(float c[3], float a[3], float b[3]);
extern LIB3DSAPI void lib3ds_vector_sub(float c[3], float a[3], float b[3]);
extern LIB3DSAPI void lib3ds_vector_scalar_mul(float c[3], float a[3], float k);
extern LIB3DSAPI void lib3ds_vector_cross(float c[3], float a[3], float b[3]);
extern LIB3DSAPI float lib3ds_vector_dot(float a[3], float b[3]);
extern LIB3DSAPI float lib3ds_vector_length(float c[3]);
extern LIB3DSAPI void lib3ds_vector_normalize(float c[3]);
extern LIB3DSAPI void lib3ds_vector_normal(float n[3], float a[3], float b[3], float c[3]);
extern LIB3DSAPI void lib3ds_vector_min(float c[3], float a[3]);
extern LIB3DSAPI void lib3ds_vector_max(float c[3], float a[3]);
extern LIB3DSAPI void lib3ds_vector_transform(float c[3], float m[4][4], float a[3]);

/** @} */
#endif
