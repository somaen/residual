/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_MATRIX_H
#define INCLUDED_LIB3DS_MATRIX_H
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
#include "common/scummsys.h"
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

extern LIB3DSAPI void lib3ds_matrix_zero(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_identity(float  m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_copy(float dest[4][4], float src[4][4]);
extern LIB3DSAPI void lib3ds_matrix_neg(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_transpose(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_add(float m[4][4], float a[4][4], float b[4][4]);
extern LIB3DSAPI void lib3ds_matrix_sub(float m[4][4], float a[4][4], float b[4][4]);
extern LIB3DSAPI void lib3ds_matrix_mult(float m[4][4], float a[4][4], float b[4][4]);
extern LIB3DSAPI void lib3ds_matrix_scalar(float m[4][4], float k);
extern LIB3DSAPI float lib3ds_matrix_det(float m[4][4]);
extern LIB3DSAPI int lib3ds_matrix_inv(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_translate(float m[4][4], float x, float y, float z);
extern LIB3DSAPI void lib3ds_matrix_scale(float m[4][4], float x, float y, float z);
extern LIB3DSAPI void lib3ds_matrix_rotate_quat(float m[4][4], float q[4]);
extern LIB3DSAPI void lib3ds_matrix_rotate(float m[4][4], float angle, float ax, float ay, float az);
extern LIB3DSAPI void lib3ds_matrix_camera(float m[4][4], float pos[3], float tgt[3], float roll);

/** @} */
#endif
