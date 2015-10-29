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

Lib3dsLight::Lib3dsLight(const Common::String &name) : _name(name) {
}


static void spotlight_read(Lib3dsLightPtr light, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_DL_SPOTLIGHT, io);

	light->spot_light = true;
	for (int i = 0; i < 3; ++i) {
		light->target.setValue(i, lib3ds_io_read_float(stream));
	}
	light->hotspot = lib3ds_io_read_float(stream);
	light->falloff = lib3ds_io_read_float(stream);
	lib3ds_chunk_read_tell(&c, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_DL_SPOT_ROLL:
			light->roll = lib3ds_io_read_float(stream);
			break;

		case CHK_DL_SHADOWED: {
			light->shadowed = true;
			break;
		}

		case CHK_DL_LOCAL_SHADOW2: {
			light->shadow_bias = lib3ds_io_read_float(stream);
			light->shadow_filter = lib3ds_io_read_float(stream);
			light->shadow_size = stream->readSint16LE();
			break;
		}

		case CHK_DL_SEE_CONE: {
			light->see_cone = true;
			break;
		}

		case CHK_DL_SPOT_RECTANGULAR: {
			light->rectangular_spot = true;
			break;
		}

		case CHK_DL_SPOT_ASPECT: {
			light->spot_aspect = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_DL_SPOT_PROJECTOR: {
			light->use_projector = true;
			lib3ds_io_read_string(io, light->projector, 64);
			break;
		}

		case CHK_DL_SPOT_OVERSHOOT: {
			light->spot_overshoot = true;
			break;
		}

		case CHK_DL_RAY_BIAS: {
			light->ray_bias = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_DL_RAYSHAD: {
			light->ray_shadows = true;
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}


void lib3ds_light_read(Lib3dsLightPtr light, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_N_DIRECT_LIGHT, io);

	{
		for (int i = 0; i < 3; ++i) {
			light->position[i] = lib3ds_io_read_float(stream);
		}
	}
	lib3ds_chunk_read_tell(&c, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_COLOR_F: {
			for (int i = 0; i < 3; ++i) {
				light->color[i] = lib3ds_io_read_float(stream);
			}
			break;
		}

		case CHK_DL_OFF:
			light->off = true;
			break;

		case CHK_DL_OUTER_RANGE:
			light->outer_range = lib3ds_io_read_float(stream);
			break;

		case CHK_DL_INNER_RANGE:
			light->inner_range = lib3ds_io_read_float(stream);
			break;

		case CHK_DL_MULTIPLIER:
			light->multiplier = lib3ds_io_read_float(stream);
			break;

		case CHK_DL_EXCLUDE: {
			/* FIXME: */
			lib3ds_chunk_unknown(chunk, io);
			break;
		}

		case CHK_DL_ATTENUATE:
			light->attenuation = lib3ds_io_read_float(stream);
			break;

		case CHK_DL_SPOTLIGHT: {
			lib3ds_chunk_read_reset(&c, io);
			spotlight_read(light, io);
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}

