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
#include <math.h>

void lib3ds_shadow_read(Lib3dsShadow *shadow, Lib3dsIo *io) {
	Lib3dsChunk c;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read(&c, io);
	switch (c.chunk) {
	case CHK_SHADOW_MAP_SIZE: {
		shadow->map_size = stream->readSint16LE();
		break;
	}

	case CHK_LO_SHADOW_BIAS: {
		shadow->low_bias = lib3ds_io_read_float(stream);
		break;
	}

	case CHK_HI_SHADOW_BIAS: {
		shadow->hi_bias = lib3ds_io_read_float(stream);
		break;
	}

	case CHK_SHADOW_FILTER: {
		shadow->filter = lib3ds_io_read_float(stream);
		break;
	}

	case CHK_RAY_BIAS: {
		shadow->ray_bias = lib3ds_io_read_float(stream);
		break;
	}
	}
}

