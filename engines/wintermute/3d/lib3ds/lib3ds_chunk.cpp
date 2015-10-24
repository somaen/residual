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
#include "lib3ds_impl.h"
#include "lib3ds_io.h"
#include <assert.h>

/*#define LIB3DS_CHUNK_DEBUG*/
/*#define LIB3DS_CHUNK_WARNING*/


/*!
 * Reads a 3d-Studio chunk header from a little endian file stream.
 *
 * \param c  The chunk to store the data.
 * \param io The file stream.
 *
 * \return   True on success, False otherwise.
 */
void lib3ds_chunk_read(Lib3dsChunk *c, Lib3dsIo *io) {
	assert(c);
	assert(io);

	Common::SeekableReadStream *stream = io->stream;

	c->cur = stream->pos();
	c->chunk = stream->readUint16LE();
	c->size = stream->readUint32LE();
	c->end = c->cur + c->size;
	c->cur += 6;
	if (c->size < 6) {
		lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Invalid chunk header.");
	}

}


void lib3ds_chunk_read_start(Lib3dsChunk *c, uint16 chunk, Lib3dsIo *io) {
	assert(c);
	assert(io);
	lib3ds_chunk_read(c, io);
	if ((chunk != 0) && (c->chunk != chunk)) {
		lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Unexpected chunk found.");
	}
	((Lib3dsIoImpl *)io->impl)->log_indent++;
}


void lib3ds_chunk_read_tell(Lib3dsChunk *c, Lib3dsIo *io) {
	c->cur = io->stream->pos();
}


uint16 lib3ds_chunk_read_next(Lib3dsChunk *c, Lib3dsIo *io) {
	Lib3dsChunk d;

	Common::SeekableReadStream *stream = io->stream;

	if (c->cur >= c->end) {
		assert(c->cur == c->end);
		return 0;
	}

	io->stream->seek(c->cur, SEEK_SET);
	d.chunk = stream->readUint16LE();
	d.size = stream->readUint32LE();
	c->cur += d.size;

	if (io->log_func) {
		lib3ds_io_log(io, LIB3DS_LOG_INFO, "%s (0x%X) size=%lu", lib3ds_chunk_name(d.chunk), d.chunk, d.size);
	}

	return d.chunk;
}


void lib3ds_chunk_read_reset(Lib3dsChunk *c, Lib3dsIo *io) {
	io->stream->seek(-6, SEEK_CUR);
}


void lib3ds_chunk_read_end(Lib3dsChunk *c, Lib3dsIo *io) {
	((Lib3dsIoImpl *)io->impl)->log_indent--;
	io->stream->seek(c->end, SEEK_SET);
}


void lib3ds_chunk_unknown(uint16 chunk, Lib3dsIo *io) {
	if (io->log_func) {
		lib3ds_io_log(io, LIB3DS_LOG_WARN, "Unknown Chunk: %s (0x%X)", lib3ds_chunk_name(chunk), chunk);
	}
}


