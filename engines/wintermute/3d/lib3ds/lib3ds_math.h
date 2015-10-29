/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_MATH_H
#define INCLUDED_LIB3DS_MATH_H
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

/**
    Calculates the ease in/out function. See Lib3dsKey for details.

    \param fp           Previous frame number.
    \param fc           Current frame number.
    \param fn           Next frame number.
    \param ease_from    Ease in value [0, 1.0]
    \param ease_to      Ease out value [0, 1.0]
*/
float lib3ds_math_ease(
    float fp,
    float fc,
    float fn,
    float ease_from,
    float ease_to);

/**
    Computes a point on a n-dimensional cubic hermite spline.
    \param v
        [out] Result
    \param a
        [in] First point of the spline.
    \param p
        [in] Tangent at the first point of the spline.
    \param q
        [in] Tangent at the second point of the spline.
    \param b
        [in] Second point of the spline.
    \param n
        [in] Dimension
    \param t
        [in] Parameter value [0...1]
*/
void lib3ds_math_cubic_interp(
    float *v,
    float *a,
    float *p,
    float *q,
    float *b,
    int n,
    float t);

/** @} */
#endif
