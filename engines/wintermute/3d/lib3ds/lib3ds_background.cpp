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
#include <string.h>
#include <math.h>

static void
solid_bgnd_read(Lib3dsBackground *background, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	int have_lin = false;

	lib3ds_chunk_read_start(&c, CHK_SOLID_BGND, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_LIN_COLOR_F:
			lib3ds_io_read_rgb(io, background->solid_color);
			have_lin = true;
			break;

		case CHK_COLOR_F:
			lib3ds_io_read_rgb(io, background->solid_color);
			break;

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}


static void
v_gradient_read(Lib3dsBackground *background, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	int index[2];
	float col[2][3][3];
	int have_lin = 0;

	lib3ds_chunk_read_start(&c, CHK_V_GRADIENT, io);

	background->gradient_percent = lib3ds_io_read_float(io);
	lib3ds_chunk_read_tell(&c, io);

	index[0] = index[1] = 0;
	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_COLOR_F:
			lib3ds_io_read_rgb(io, col[0][index[0]]);
			index[0]++;
			break;

		case CHK_LIN_COLOR_F:
			lib3ds_io_read_rgb(io, col[1][index[1]]);
			index[1]++;
			have_lin = 1;
			break;

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}
	{
		int i;
		for (i = 0; i < 3; ++i) {
			background->gradient_top[i] = col[have_lin][0][i];
			background->gradient_middle[i] = col[have_lin][1][i];
			background->gradient_bottom[i] = col[have_lin][2][i];
		}
	}
	lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_background_read(Lib3dsBackground *background, Lib3dsIo *io) {
	Lib3dsChunk c;

	lib3ds_chunk_read(&c, io);
	switch (c.chunk) {
	case CHK_BIT_MAP: {
		lib3ds_io_read_string(io, background->bitmap_name, 64);
		break;
	}

	case CHK_SOLID_BGND: {
		lib3ds_chunk_read_reset(&c, io);
		solid_bgnd_read(background, io);
		break;
	}

	case CHK_V_GRADIENT: {
		lib3ds_chunk_read_reset(&c, io);
		v_gradient_read(background, io);
		break;
	}

	case CHK_USE_BIT_MAP: {
		background->use_bitmap = true;
		break;
	}

	case CHK_USE_SOLID_BGND: {
		background->use_solid = true;
		break;
	}

	case CHK_USE_V_GRADIENT: {
		background->use_gradient = true;
		break;
	}
	}
}


static int
colorf_defined(float rgb[3]) {
	int i;
	for (i = 0; i < 3; ++i) {
		if (fabs(rgb[i]) > LIB3DS_EPSILON) {
			break;
		}
	}
	return (i < 3);
}
