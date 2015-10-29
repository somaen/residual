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
#include "lib3ds_io.h"
#include "lib3ds_node.h"
#include <string.h>
#include <assert.h>

typedef union {
	uint32 dword_value;
	float float_value;
} Lib3dsDwordFloat;


void lib3ds_io_setup(Lib3dsIo *io) {
	assert(io);
	io->impl = calloc(sizeof(Lib3dsIoImpl), 1);
}


void lib3ds_io_cleanup(Lib3dsIo *io) {
	Lib3dsIoImpl *impl;
	assert(io);
	impl = (Lib3dsIoImpl *)io->impl;
	if (impl->tmp_mem) {
		free(impl->tmp_mem);
		impl->tmp_mem = NULL;
	}
	if (impl->tmp_node) {
		lib3ds_node_free(impl->tmp_node);
		impl->tmp_node = NULL;
	}
	free(impl);
}

static void lib3ds_io_log_str(Lib3dsIo *io, Lib3dsLogLevel level, const char *str) {
	if (!io || !io->log_func)
		return;
	(*io->log_func)(io->stream, level, ((Lib3dsIoImpl *)io->impl)->log_indent, str);
}


void lib3ds_io_log(Lib3dsIo *io, Lib3dsLogLevel level, const char *format, ...) {
	va_list args;
	/* FIXME */ char str[1024];

	assert(io);
	if (!io || !io->log_func)
		return;

	va_start(args, format);
	/* FIXME: */ vsprintf(str, format, args);
	lib3ds_io_log_str(io, level, str);

	if (level == LIB3DS_LOG_ERROR) {
		longjmp(((Lib3dsIoImpl *)io->impl)->jmpbuf, 1);
	}
}


void lib3ds_io_log_indent(Lib3dsIo *io, int indent) {
	assert(io);
	if (!io)
		return;
	((Lib3dsIoImpl *)io->impl)->log_indent += indent;
}


void lib3ds_io_read_error(Lib3dsIo *io) {
	lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Reading from input stream failed.");
}


/*!
 * Read a float from a file stream in little endian format.
 */
float lib3ds_io_read_float(Common::SeekableReadStream *stream) {
	uint8 b[4];
	Lib3dsDwordFloat d;

	assert(stream);
	stream->read(b, 4);
	d.dword_value = ((uint32)b[3] << 24) |
	                ((uint32)b[2] << 16) |
	                ((uint32)b[1] << 8) |
	                ((uint32)b[0]);
	return d.float_value;
}


/*!
 * Read a vector from a file stream in little endian format.
 *
 * \param io IO input handle.
 * \param v  The vector to store the data.
 */
void lib3ds_io_read_vector(Common::SeekableReadStream *stream, Math::Vector3d &v) {
	assert(stream);
	v.setValue(0, lib3ds_io_read_float(stream));
	v.setValue(1, lib3ds_io_read_float(stream));
	v.setValue(2, lib3ds_io_read_float(stream));
}


void lib3ds_io_read_rgb(Common::SeekableReadStream *stream, float rgb[3]) {
	assert(stream);
	rgb[0] = lib3ds_io_read_float(stream);
	rgb[1] = lib3ds_io_read_float(stream);
	rgb[2] = lib3ds_io_read_float(stream);
}


/*!
 * Read a zero-terminated string from a file stream.
 *
 * \param io      IO input handle.
 * \param s       The buffer to store the read string.
 * \param buflen  Buffer length.
 *
 * \return        True on success, False otherwise.
 */
void lib3ds_io_read_string(Lib3dsIo *io, char *s, int buflen) {
	char c;
	int k = 0;

	Common::SeekableReadStream *stream = io->stream;

	assert(io);
	for (;;) {
		if (stream->read(&c, 1) != 1) {
			lib3ds_io_read_error(io);
		}
		*s++ = c;
		if (!c) {
			break;
		}
		++k;
		if (k >= buflen) {
			lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Invalid string in input stream.");
		}
	}
}

void lib3ds_io_read_string(Lib3dsIo *io, Common::String &s, int buflen) {
	char *tmp = new char[buflen + 1];
	lib3ds_io_read_string(io, tmp, buflen);
	s = tmp;
	delete[] tmp;
}

