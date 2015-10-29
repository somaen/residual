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
#include <assert.h>
#include <math.h>

static void initialize_texture_map(Lib3dsTextureMap *map) {
	map->flags = 0x10;
	map->percent = 1.0f;
	map->scale[0] = 1.0f;
	map->scale[1] = 1.0f;
}


/*!
 * Constructs a new, empty Lib3dsMaterial object.
 *
 * Initial value of the material is a shiny grey.
 *
 * \return A pointer to the Lib3dsMaterial structure.
 *  If the structure cannot be allocated, NULL is returned.
 */
Lib3dsMaterial::Lib3dsMaterial(const Common::String &name) : _name(name) {
	ambient[0] = ambient[1] = ambient[2] = 0.588235f;
	diffuse[0] = diffuse[1] = diffuse[2] = 0.588235f;
	specular[0] = specular[1] = specular[2] = 0.898039f;
	shininess = 0.1f;
	wire_size = 1.0f;
	shading = 3;

	initialize_texture_map(&texture1_map);
	initialize_texture_map(&texture1_mask);
	initialize_texture_map(&texture2_map);
	initialize_texture_map(&texture2_mask);
	initialize_texture_map(&opacity_map);
	initialize_texture_map(&opacity_mask);
	initialize_texture_map(&bump_map);
	initialize_texture_map(&bump_mask);
	initialize_texture_map(&specular_map);
	initialize_texture_map(&specular_mask);
	initialize_texture_map(&shininess_map);
	initialize_texture_map(&shininess_mask);
	initialize_texture_map(&self_illum_map);
	initialize_texture_map(&self_illum_mask);
	initialize_texture_map(&reflection_map);
	initialize_texture_map(&reflection_mask);
}


static void color_read(float rgb[3], Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	int have_lin = false;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, 0, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_LIN_COLOR_24: {
			for (int i = 0; i < 3; ++i) {
				rgb[i] = 1.0f * stream->readByte() / 255.0f;
			}
			have_lin = true;
			break;
		}

		case CHK_COLOR_24: {
			/* gamma corrected color chunk
			   replaced in 3ds R3 by LIN_COLOR_24 */
			if (!have_lin) {
				for (int i = 0; i < 3; ++i) {
					rgb[i] = 1.0f * stream->readByte() / 255.0f;
				}
			}
			break;
		}

		case CHK_LIN_COLOR_F: {
			for (int i = 0; i < 3; ++i) {
				rgb[i] = lib3ds_io_read_float(stream);
			}
			have_lin = true;
			break;
		}

		case CHK_COLOR_F: {
			if (!have_lin) {
				for (int i = 0; i < 3; ++i) {
					rgb[i] = lib3ds_io_read_float(stream);
				}
			}
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}


static void int_percentage_read(float *p, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;
	
	lib3ds_chunk_read_start(&c, 0, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_INT_PERCENTAGE: {
			int16 i = stream->readSint16LE();
			*p = (float)(1.0 * i / 100.0);
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}


static void texture_map_read(Lib3dsTextureMap *map, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, 0, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_INT_PERCENTAGE: {
			map->percent = 1.0f * stream->readSint16LE() / 100.0f;
			break;
		}

		case CHK_MAT_MAPNAME: {
			lib3ds_io_read_string(io, map->name, 64);
			lib3ds_io_log(io, LIB3DS_LOG_INFO, "  NAME=%s", map->name);
			break;
		}

		case CHK_MAT_MAP_TILING: {
			map->flags = stream->readUint16LE();
			break;
		}

		case CHK_MAT_MAP_TEXBLUR:
			map->blur = lib3ds_io_read_float(stream);
			break;

		case CHK_MAT_MAP_USCALE:
			map->scale[0] = lib3ds_io_read_float(stream);
			break;

		case CHK_MAT_MAP_VSCALE: {
			map->scale[1] = lib3ds_io_read_float(stream);
			break;
		}
		case CHK_MAT_MAP_UOFFSET: {
			map->offset[0] = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_MAT_MAP_VOFFSET: {
			map->offset[1] = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_MAT_MAP_ANG: {
			map->rotation = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_MAT_MAP_COL1: {
			map->tint_1[0] = 1.0f * stream->readByte() / 255.0f;
			map->tint_1[1] = 1.0f * stream->readByte() / 255.0f;
			map->tint_1[2] = 1.0f * stream->readByte() / 255.0f;
			break;
		}

		case CHK_MAT_MAP_COL2: {
			map->tint_2[0] = 1.0f * stream->readByte() / 255.0f;
			map->tint_2[1] = 1.0f * stream->readByte() / 255.0f;
			map->tint_2[2] = 1.0f * stream->readByte() / 255.0f;
			break;
		}

		case CHK_MAT_MAP_RCOL: {
			map->tint_r[0] = 1.0f * stream->readByte() / 255.0f;
			map->tint_r[1] = 1.0f * stream->readByte() / 255.0f;
			map->tint_r[2] = 1.0f * stream->readByte() / 255.0f;
			break;
		}

		case CHK_MAT_MAP_GCOL: {
			map->tint_g[0] = 1.0f * stream->readByte() / 255.0f;
			map->tint_g[1] = 1.0f * stream->readByte() / 255.0f;
			map->tint_g[2] = 1.0f * stream->readByte() / 255.0f;
			break;
		}

		case CHK_MAT_MAP_BCOL: {
			map->tint_b[0] = 1.0f * stream->readByte() / 255.0f;
			map->tint_b[1] = 1.0f * stream->readByte() / 255.0f;
			map->tint_b[2] = 1.0f * stream->readByte() / 255.0f;
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}


void lib3ds_material_read(Lib3dsMaterialPtr material, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	assert(material);
	lib3ds_chunk_read_start(&c, CHK_MAT_ENTRY, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_MAT_NAME: {
			lib3ds_io_read_string(io, material->_name, 64);
			lib3ds_io_log(io, LIB3DS_LOG_INFO, "  NAME=%s", material->_name.c_str());
			break;
		}

		case CHK_MAT_AMBIENT: {
			lib3ds_chunk_read_reset(&c, io);
			color_read(material->ambient, io);
			break;
		}

		case CHK_MAT_DIFFUSE: {
			lib3ds_chunk_read_reset(&c, io);
			color_read(material->diffuse, io);
			break;
		}

		case CHK_MAT_SPECULAR: {
			lib3ds_chunk_read_reset(&c, io);
			color_read(material->specular, io);
			break;
		}

		case CHK_MAT_SHININESS: {
			lib3ds_chunk_read_reset(&c, io);
			int_percentage_read(&material->shininess, io);
			break;
		}

		case CHK_MAT_SHIN2PCT: {
			lib3ds_chunk_read_reset(&c, io);
			int_percentage_read(&material->shin_strength, io);
			break;
		}

		case CHK_MAT_TRANSPARENCY: {
			lib3ds_chunk_read_reset(&c, io);
			int_percentage_read(&material->transparency, io);
			break;
		}

		case CHK_MAT_XPFALL: {
			lib3ds_chunk_read_reset(&c, io);
			int_percentage_read(&material->falloff, io);
			break;
		}

		case CHK_MAT_SELF_ILPCT: {
			lib3ds_chunk_read_reset(&c, io);
			int_percentage_read(&material->self_illum, io);
			break;
		}

		case CHK_MAT_USE_XPFALL: {
			material->use_falloff = true;
			break;
		}

		case CHK_MAT_REFBLUR: {
			lib3ds_chunk_read_reset(&c, io);
			int_percentage_read(&material->blur, io);
			break;
		}

		case CHK_MAT_USE_REFBLUR: {
			material->use_blur = true;
			break;
		}

		case CHK_MAT_SHADING: {
			material->shading = stream->readSint16LE();
			break;
		}

		case CHK_MAT_SELF_ILLUM: {
			material->self_illum_flag = true;
			break;
		}

		case CHK_MAT_TWO_SIDE: {
			material->two_sided = true;
			break;
		}

		case CHK_MAT_DECAL: {
			material->map_decal = true;
			break;
		}

		case CHK_MAT_ADDITIVE: {
			material->is_additive = true;
			break;
		}

		case CHK_MAT_FACEMAP: {
			material->face_map = true;
			break;
		}

		case CHK_MAT_PHONGSOFT: {
			material->soften = true;
			break;
		}

		case CHK_MAT_WIRE: {
			material->use_wire = true;
			break;
		}

		case CHK_MAT_WIREABS: {
			material->use_wire_abs = true;
			break;
		}
		case CHK_MAT_WIRE_SIZE: {
			material->wire_size = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_MAT_TEXMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->texture1_map, io);
			break;
		}

		case CHK_MAT_TEXMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->texture1_mask, io);
			break;
		}

		case CHK_MAT_TEX2MAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->texture2_map, io);
			break;
		}

		case CHK_MAT_TEX2MASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->texture2_mask, io);
			break;
		}

		case CHK_MAT_OPACMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->opacity_map, io);
			break;
		}

		case CHK_MAT_OPACMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->opacity_mask, io);
			break;
		}

		case CHK_MAT_BUMPMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->bump_map, io);
			break;
		}
		case CHK_MAT_BUMPMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->bump_mask, io);
			break;
		}
		case CHK_MAT_SPECMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->specular_map, io);
			break;
		}

		case CHK_MAT_SPECMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->specular_mask, io);
			break;
		}

		case CHK_MAT_SHINMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->shininess_map, io);
			break;
		}

		case CHK_MAT_SHINMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->shininess_mask, io);
			break;
		}

		case CHK_MAT_SELFIMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->self_illum_map, io);
			break;
		}

		case CHK_MAT_SELFIMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->self_illum_mask, io);
			break;
		}

		case CHK_MAT_REFLMAP: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->reflection_map, io);
			break;
		}

		case CHK_MAT_REFLMASK: {
			lib3ds_chunk_read_reset(&c, io);
			texture_map_read(&material->reflection_mask, io);
			break;
		}

		case CHK_MAT_ACUBIC: {
			stream->readSByte();
			material->autorefl_map_anti_alias = stream->readSByte();
			material->autorefl_map_flags = stream->readSint16LE();
			material->autorefl_map_size = stream->readSint32LE();
			material->autorefl_map_frame_step = stream->readSint32LE();
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}

