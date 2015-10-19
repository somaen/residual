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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

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


long lib3ds_io_seek(Lib3dsIo *io, long offset, int whence) {
	assert(io);
	if (!io) {
		return 0;
	}
	return io->stream->seek(offset, whence);
}


long lib3ds_io_tell(Lib3dsIo *io) {
	assert(io);
	if (!io) {
		return 0;
	}
	return io->stream->pos();
}


size_t lib3ds_io_read(Lib3dsIo *io, void *buffer, size_t size) {
	assert(io);
	if (!io) {
		return 0;
	}
	return io->stream->read(buffer, size);
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
 * Read a byte from a file stream.
 */
uint8 lib3ds_io_read_byte(Lib3dsIo *io) {
	uint8 b;

	assert(io);
	lib3ds_io_read(io, &b, 1);
	return (b);
}


/**
 * Read a word from a file stream in little endian format.
 */
uint16 lib3ds_io_read_word(Lib3dsIo *io) {
	assert(io);
	return io->stream->readUint16LE();
}


/*!
 * Read a dword from file a stream in little endian format.
 */
uint32 lib3ds_io_read_dword(Lib3dsIo *io) {
	assert(io);
	return io->stream->readUint32LE();
}


/*!
 * Read a signed byte from a file stream.
 */
int8 lib3ds_io_read_intb(Lib3dsIo *io) {
	int8 b;

	assert(io);
	lib3ds_io_read(io, &b, 1);
	return (b);
}


/*!
 * Read a float from a file stream in little endian format.
 */
float lib3ds_io_read_float(Lib3dsIo *io) {
	uint8 b[4];
	Lib3dsDwordFloat d;

	assert(io);
	lib3ds_io_read(io, b, 4);
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
void lib3ds_io_read_vector(Lib3dsIo *io, float v[3]) {
	assert(io);
	v[0] = lib3ds_io_read_float(io);
	v[1] = lib3ds_io_read_float(io);
	v[2] = lib3ds_io_read_float(io);
}


void lib3ds_io_read_rgb(Lib3dsIo *io, float rgb[3]) {
	assert(io);
	rgb[0] = lib3ds_io_read_float(io);
	rgb[1] = lib3ds_io_read_float(io);
	rgb[2] = lib3ds_io_read_float(io);
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

	assert(io);
	for (;;) {
		if (lib3ds_io_read(io, &c, 1) != 1) {
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

