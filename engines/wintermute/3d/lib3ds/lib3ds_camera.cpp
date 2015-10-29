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

/*!
 * Return a new Lib3dsCamera object.
 *
 * Object is initialized with the given name and fov=45.  All other
 * values are 0.
 *
 * \param name Name of this camera.  Must not be NULL.  Must be < 64 characters.
 *
 * \return Lib3dsCamera object or NULL on failure.
 */
Lib3dsCamera::Lib3dsCamera(const char *name) {
	assert(name);
	assert(strlen(name) < 64);

	strcpy(this->_name, name);
	_fov = 45.0f;
}


/*!
 * Free a Lib3dsCamera object and all of its resources.
 */
Lib3dsCamera::~Lib3dsCamera() {
}


/*!
 * Read a camera definition from a file.
 *
 * This function is called by lib3ds_file_read(), and you probably
 * don't want to call it directly.
 *
 * \param camera A Lib3dsCamera to be filled in.
 * \param io A Lib3dsIo object previously set up by the caller.
 *
 * \see lib3ds_file_read
 */
void lib3ds_camera_read(Lib3dsCameraPtr camera, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_N_CAMERA, io);

	{
		for (int i = 0; i < 3; ++i) {
			camera->_position[i] = lib3ds_io_read_float(stream);
		}
		for (int i = 0; i < 3; ++i) {
			camera->_target[i] = lib3ds_io_read_float(stream);
		}
	}
	camera->_roll = lib3ds_io_read_float(stream);
	{
		float s;
		s = lib3ds_io_read_float(stream);
		if (fabs(s) < LIB3DS_EPSILON) {
			camera->_fov = 45.0;
		} else {
			camera->_fov = 2400.0f / s;
		}
	}
	lib3ds_chunk_read_tell(&c, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_CAM_SEE_CONE: {
			camera->_seeCone = true;
		}
		break;

		case CHK_CAM_RANGES: {
			camera->_nearRange = lib3ds_io_read_float(stream);
			camera->_farRange = lib3ds_io_read_float(stream);
		}
		break;

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}

