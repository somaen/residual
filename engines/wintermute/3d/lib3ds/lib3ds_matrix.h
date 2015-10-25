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

extern LIB3DSAPI void lib3ds_matrix_zero(Math::Matrix4 &m);
extern LIB3DSAPI void lib3ds_matrix_identity(Math::Matrix4 &m);
extern LIB3DSAPI void lib3ds_matrix_copy(Math::Matrix4 &dest, const Math::Matrix4 &src);
extern LIB3DSAPI void lib3ds_matrix_neg(Math::Matrix4 &m);
extern LIB3DSAPI void lib3ds_matrix_transpose(Math::Matrix4 &m);
extern LIB3DSAPI void lib3ds_matrix_add(Math::Matrix4 &m, Math::Matrix4 &a, Math::Matrix4 &b);
extern LIB3DSAPI void lib3ds_matrix_sub(Math::Matrix4 &m, Math::Matrix4 &a, Math::Matrix4 &b);
extern LIB3DSAPI void lib3ds_matrix_mult(Math::Matrix4 &m, Math::Matrix4 &a, Math::Matrix4 &b);
extern LIB3DSAPI void lib3ds_matrix_scalar(Math::Matrix4 &m, float k);
extern LIB3DSAPI float lib3ds_matrix_det(Math::Matrix4 &m);
extern LIB3DSAPI int lib3ds_matrix_inv(Math::Matrix4 &m);
extern LIB3DSAPI void lib3ds_matrix_translate(Math::Matrix4 &m, float x, float y, float z);
extern LIB3DSAPI void lib3ds_matrix_scale(Math::Matrix4 &m, float x, float y, float z);
extern LIB3DSAPI void lib3ds_matrix_rotate_quat(Math::Matrix4 &m, float q[4]);
extern LIB3DSAPI void lib3ds_matrix_rotate(Math::Matrix4 &m, float angle, float ax, float ay, float az);
extern LIB3DSAPI void lib3ds_matrix_camera(Math::Matrix4 &m, float pos[3], float tgt[3], float roll);

/** @} */
#endif
