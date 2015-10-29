/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_IO_H
#define INCLUDED_LIB3DS_IO_H
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

/** @file lib3ds_io.h
    Private header file used internally by lib3ds */
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/scummsys.h"
#include "lib3ds_impl.h"

struct Lib3dsNode;

struct Lib3dsIo {
	void   *impl;
	Common::SeekableReadStream *stream;
	void (*log_func)(Common::SeekableReadStream *self, Lib3dsLogLevel level, int indent, const char *msg);
};

struct Lib3dsIoImpl {
	int log_indent;
	void *tmp_mem;
	Lib3dsNode *tmp_node;
};

void lib3ds_io_setup(Lib3dsIo *io);
void lib3ds_io_cleanup(Lib3dsIo *io);

void lib3ds_io_log(Lib3dsIo *io, Lib3dsLogLevel level, const char *format, ...);
void lib3ds_io_log_indent(Lib3dsIo *io, int indent);
void lib3ds_io_read_error(Lib3dsIo *io);

float lib3ds_io_read_float(Common::SeekableReadStream *stream);
void lib3ds_io_read_vector(Common::SeekableReadStream *stream, Math::Vector3d &);
void lib3ds_io_read_rgb(Common::SeekableReadStream *stream, float rgb[3]);
void lib3ds_io_read_string(Lib3dsIo *io, char *s, int buflen);
void lib3ds_io_read_string(Lib3dsIo *io, Common::String &s, int buflen);

#endif
