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
#include "lib3ds_matrix.h"
#include "lib3ds_node.h"
#include "lib3ds_vector.h"
#include <float.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/*!
 * Loads a .3DS file from disk into memory.
 *
 * \param stream  An open read stream to the .3DS file
 *
 * \return   A pointer to the Lib3dsFile structure containing the
 *           data of the .3DS file.
 *           If the .3DS file can not be loaded NULL is returned.
 *
 * \note     To free the returned structure use lib3ds_free.
 *
 * \see      lib3ds_file_save,
 *           lib3ds_file_new,
 *           lib3ds_file_free
 */
Lib3dsFile *lib3ds_file_open(Common::SeekableReadStream *stream) {
	if (!stream) {
		return NULL;
	}
	Lib3dsFile *file = new Lib3dsFile();
	if (!file) {
		// Not sure we should do this
		delete stream;
		return NULL;
	}

	Lib3dsIo io;
	memset(&io, 0, sizeof(io));
	
	io.stream = stream;
	io.log_func = NULL;

	if (!lib3ds_file_read(file, &io)) {
		// Not sure we should do this
		delete stream;
		return NULL;
	}

	// Not sure we should do this
	delete stream;
	return file;
}


/*!
 * Constructs a new, empty Lib3dsFile object.
 */
Lib3dsFile::Lib3dsFile() {
	_meshVersion = 3;
	_masterScale = 1.0f;
	_keyfRevision = 5;
	_name = "LIB3DS";

	_frames = 100;
	_segmentFrom = 0;
	_segmentTo = 100;
	_currentFrame = 0;
	_userId = 0;
	_userPtr = 0;
	_frames = 0;
	_segmentFrom = 0;
	_segmentTo = 0;
	_currentFrame = 0;
	_materialsSize = 0;
	_nmaterials = 0;
	_materials = 0;
	_camerasSize = 0;
	_ncameras = 0;
	_cameras = 0;
	_lightsSize = 0;
	_nlights = 0;
	_lights = 0;
	_meshesSize = 0;
	_nmeshes = 0;
	_nodes = 0;
	
	for (int i = 0; i < 3; i++) {
		_constructionPlane[i] = 0;
		_ambient[i] = 0;
	}
}


/*!
 * Free a Lib3dsFile object and all of its resources.
 */
Lib3dsFile::~Lib3dsFile() {
	reserveMaterials(0, true);
	reserveCameras(0, true);
	reserveLights(0, true);
	{
		Lib3dsNode *p, *q;

		for (p = _nodes; p; p = q) {
			q = p->next;
			lib3ds_node_free(p);
		}
	}
}


/*!
 * Evaluate all of the nodes in this Lib3dsFile object.
 *
 * \param file The Lib3dsFile object to be evaluated.
 * \param t time value, between 0. and file->frames
 *
 * \see lib3ds_node_eval
 */
void lib3ds_file_eval(Lib3dsFile *file, float t) {
	for (Lib3dsNode *p = file->_nodes; p != 0; p = p->next) {
		lib3ds_node_eval(p, t);
	}
}


static void named_object_read(Lib3dsFile *file, Lib3dsIo *io) {
	Lib3dsChunk c;
	char name[64];
	uint16 chunk;
	Lib3dsMeshPtr mesh;
	Lib3dsCamera *camera = NULL;
	Lib3dsLight *light = NULL;
	uint32 objectFlags;

	lib3ds_chunk_read_start(&c, CHK_NAMED_OBJECT, io);

	lib3ds_io_read_string(io, name, 64);
	lib3ds_io_log(io, LIB3DS_LOG_INFO, "  NAME=%s", name);
	lib3ds_chunk_read_tell(&c, io);

	objectFlags = 0;
	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_N_TRI_OBJECT: {
			Lib3dsMeshPtr mesh(new Lib3dsMesh(name));
			file->insertMesh(mesh, -1);
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_mesh_read(file, mesh, io);
			break;
		}

		case CHK_N_CAMERA: {
			camera = new Lib3dsCamera(name);
			file->insertCamera(camera, -1);
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_camera_read(camera, io);
			break;
		}

		case CHK_N_DIRECT_LIGHT: {
			light = new Lib3dsLight(name);
			file->insertLight(light, -1);
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_light_read(light, io);
			break;
		}

		case CHK_OBJ_HIDDEN:
			objectFlags |= LIB3DS_OBJECT_HIDDEN;
			break;

		case CHK_OBJ_DOESNT_CAST:
			objectFlags |= LIB3DS_OBJECT_DOESNT_CAST;
			break;

		case CHK_OBJ_VIS_LOFTER:
			objectFlags |= LIB3DS_OBJECT_VIS_LOFTER;
			break;

		case CHK_OBJ_MATTE:
			objectFlags |= LIB3DS_OBJECT_MATTE;
			break;

		case CHK_OBJ_DONT_RCVSHADOW:
			objectFlags |= LIB3DS_OBJECT_DONT_RCVSHADOW;
			break;

		case CHK_OBJ_FAST:
			objectFlags |= LIB3DS_OBJECT_FAST;
			break;

		case CHK_OBJ_FROZEN:
			objectFlags |= LIB3DS_OBJECT_FROZEN;
			break;

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	if (mesh)
		mesh->_objectFlags = objectFlags;
	if (camera)
		camera->_objectFlags = objectFlags;
	if (light)
		light->object_flags = objectFlags;

	lib3ds_chunk_read_end(&c, io);
}


static void ambient_read(Lib3dsFile *file, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	int have_lin = false;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_AMBIENT_LIGHT, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_LIN_COLOR_F: {
			for (int i = 0; i < 3; ++i) {
				file->_ambient[i] = lib3ds_io_read_float(stream);
			}
			have_lin = true;
			break;
		}

		case CHK_COLOR_F: {
			/* gamma corrected color chunk
			   replaced in 3ds R3 by LIN_COLOR_24 */
			if (!have_lin) {
				for (int i = 0; i < 3; ++i) {
					file->_ambient[i] = lib3ds_io_read_float(stream);
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


static void mdata_read(Lib3dsFile *file, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_MDATA, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_MESH_VERSION: {
			file->_meshVersion = stream->readSint32LE();
			break;
		}

		case CHK_MASTER_SCALE: {
			file->_masterScale = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_SHADOW_MAP_SIZE:
		case CHK_LO_SHADOW_BIAS:
		case CHK_HI_SHADOW_BIAS:
		case CHK_SHADOW_SAMPLES:
		case CHK_SHADOW_RANGE:
		case CHK_SHADOW_FILTER:
		case CHK_RAY_BIAS: {
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_shadow_read(&file->_shadow, io);
			break;
		}

		case CHK_VIEWPORT_LAYOUT:
		case CHK_DEFAULT_VIEW: {
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_viewport_read(&file->_viewport, io);
			break;
		}

		case CHK_O_CONSTS: {
			for (int i = 0; i < 3; ++i) {
				file->_constructionPlane[i] = lib3ds_io_read_float(stream);
			}
			break;
		}

		case CHK_AMBIENT_LIGHT: {
			lib3ds_chunk_read_reset(&c, io);
			ambient_read(file, io);
			break;
		}

		case CHK_BIT_MAP:
		case CHK_SOLID_BGND:
		case CHK_V_GRADIENT:
		case CHK_USE_BIT_MAP:
		case CHK_USE_SOLID_BGND:
		case CHK_USE_V_GRADIENT: {
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_background_read(&file->_background, io);
			break;
		}

		case CHK_FOG:
		case CHK_LAYER_FOG:
		case CHK_DISTANCE_CUE:
		case CHK_USE_FOG:
		case CHK_USE_LAYER_FOG:
		case CHK_USE_DISTANCE_CUE: {
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_atmosphere_read(&file->_atmosphere, io);
			break;
		}

		case CHK_MAT_ENTRY: {
			Lib3dsMaterial *material =  new Lib3dsMaterial(NULL);
			file->insertMaterial(material, -1);
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_material_read(material, io);
			break;
		}

		case CHK_NAMED_OBJECT: {
			lib3ds_chunk_read_reset(&c, io);
			named_object_read(file, io);
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}


static int compare_node_id(const void *a, const void *b) {
	return (*((Lib3dsNode **)a))->node_id - (*((Lib3dsNode **)b))->node_id;
}


static int compare_node_id2(const void *a, const void *b) {
	return *((unsigned short *)a) - (*((Lib3dsNode **)b))->node_id;
}


static void kfdata_read(Lib3dsFile *file, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	unsigned num_nodes = 0;
	Lib3dsNode *last = NULL;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_KFDATA, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_KFHDR: {
			file->_keyfRevision = stream->readUint16LE();
			lib3ds_io_read_string(io, file->_name, 12 + 1);
			file->_frames = stream->readSint32LE();
			break;
		}

		case CHK_KFSEG: {
			file->_segmentFrom = stream->readSint32LE();
			file->_segmentTo = stream->readSint32LE();
			break;
		}

		case CHK_KFCURTIME: {
			file->_currentFrame = stream->readSint32LE();
			break;
		}

		case CHK_VIEWPORT_LAYOUT:
		case CHK_DEFAULT_VIEW: {
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_viewport_read(&file->_viewportKeyf, io);
			break;
		}

		case CHK_AMBIENT_NODE_TAG:
		case CHK_OBJECT_NODE_TAG:
		case CHK_CAMERA_NODE_TAG:
		case CHK_TARGET_NODE_TAG:
		case CHK_LIGHT_NODE_TAG:
		case CHK_SPOTLIGHT_NODE_TAG:
		case CHK_L_TARGET_NODE_TAG: {
			Lib3dsNodeType type = (Lib3dsNodeType)0;
			Lib3dsNode *node;

			switch (chunk) {
			case CHK_AMBIENT_NODE_TAG:
				type = LIB3DS_NODE_AMBIENT_COLOR;
				break;
			case CHK_OBJECT_NODE_TAG:
				type = LIB3DS_NODE_MESH_INSTANCE;
				break;
			case CHK_CAMERA_NODE_TAG:
				type = LIB3DS_NODE_CAMERA;
				break;
			case CHK_TARGET_NODE_TAG:
				type = LIB3DS_NODE_CAMERA_TARGET;
				break;
			case CHK_LIGHT_NODE_TAG:
				type = LIB3DS_NODE_OMNILIGHT;
				break;
			case CHK_SPOTLIGHT_NODE_TAG:
				type = LIB3DS_NODE_SPOTLIGHT;
				break;
			case CHK_L_TARGET_NODE_TAG:
				type = LIB3DS_NODE_SPOTLIGHT_TARGET;
				break;
			}

			node = lib3ds_node_new(type);
			node->node_id = (unsigned short)(num_nodes++);
			if (last) {
				last->next = node;
			} else {
				file->_nodes = node;
			}
			node->user_ptr = last;
			last = node;
			lib3ds_chunk_read_reset(&c, io);
			lib3ds_node_read(node, io);
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	{
		Lib3dsNode **nodes = (Lib3dsNode **)malloc(num_nodes * sizeof(Lib3dsNode *));
		unsigned i;
		Lib3dsNode *p, *q, *parent;

		p = file->_nodes;
		for (i = 0; i < num_nodes; ++i) {
			nodes[i] = p;
			p = p->next;
		}
		qsort(nodes, num_nodes, sizeof(Lib3dsNode *), compare_node_id);

		p = last;
		while (p) {
			q = (Lib3dsNode *)p->user_ptr;
			if (p->user_id != 65535) {
				parent = *(Lib3dsNode **)bsearch(&p->user_id, nodes, num_nodes, sizeof(Lib3dsNode *), compare_node_id2);
				if (parent) {
					q->next = p->next;
					p->next = parent->childs;
					p->parent = parent;
					parent->childs = p;
				} else {
					/* TODO: warning */
				}
			}
			p->user_id = 0;
			p->user_ptr = NULL;
			p = q;
		}
		free(nodes);
	}

	lib3ds_chunk_read_end(&c, io);
}


/*!
 * Read 3ds file data into a Lib3dsFile object.
 *
 * \param file The Lib3dsFile object to be filled.
 * \param io A Lib3dsIo object previously set up by the caller.
 *
 * \return LIB3DS_TRUE on success, LIB3DS_FALSE on failure.
 */
int lib3ds_file_read(Lib3dsFile *file, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	Lib3dsIoImpl *impl;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_io_setup(io);
	impl = (Lib3dsIoImpl *)io->impl;

	if (setjmp(impl->jmpbuf) != 0) {
		lib3ds_io_cleanup(io);
		return false;
	}

	lib3ds_chunk_read_start(&c, 0, io);
	switch (c.chunk) {
	case CHK_MDATA: {
		lib3ds_chunk_read_reset(&c, io);
		mdata_read(file, io);
		break;
	}

	case CHK_M3DMAGIC:
	case CHK_MLIBMAGIC:
	case CHK_CMAGIC: {
		while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
			switch (chunk) {
			case CHK_M3D_VERSION: {
				file->_meshVersion = stream->readUint32LE();
				break;
			}

			case CHK_MDATA: {
				lib3ds_chunk_read_reset(&c, io);
				mdata_read(file, io);
				break;
			}

			case CHK_KFDATA: {
				lib3ds_chunk_read_reset(&c, io);
				kfdata_read(file, io);
				break;
			}

			default:
				lib3ds_chunk_unknown(chunk, io);
			}
		}
		break;
	}

	default:
		lib3ds_chunk_unknown(c.chunk, io);
		return false;
	}

	lib3ds_chunk_read_end(&c, io);

	memset(impl->jmpbuf, 0, sizeof(impl->jmpbuf));
	lib3ds_io_cleanup(io);
	return true;
}


void Lib3dsFile::reserveMaterials(int size, int force) {
	lib3ds_util_reserve_array((void ** *)&_materials, &_nmaterials, &_materialsSize,
	                          size, force, (Lib3dsFreeFunc)lib3ds_material_free);
}


void Lib3dsFile::insertMaterial(Lib3dsMaterial *material, int index) {
	lib3ds_util_insert_array((void ** *)&_materials, &_nmaterials, &_materialsSize, material, index);
}


void Lib3dsFile::removeMaterial(int index) {
	lib3ds_util_remove_array((void ** *)&_materials, &_nmaterials, index, (Lib3dsFreeFunc)lib3ds_material_free);
}


int Lib3dsFile::materialByName(const char *name) {
	for (int i = 0; i < _nmaterials; ++i) {
		if (strcmp(_materials[i]->name, name) == 0) {
			return (i);
		}
	}
	return -1;
}


void Lib3dsFile::reserveCameras(int size, int force) {
	lib3ds_util_reserve_array_delete(&_cameras, &_ncameras, &_camerasSize, size, force);
}


void Lib3dsFile::insertCamera(Lib3dsCamera *camera, int index) {
	lib3ds_util_insert_array((void ** *)&_cameras, &_ncameras, &_camerasSize, camera, index);
}


void Lib3dsFile::removeCamera(int index) {
	lib3ds_util_remove_array_delete(&_cameras, &_ncameras, index);
}


int Lib3dsFile::cameraByName(const char *name) {
	for (int i = 0; i < _ncameras; ++i) {
		if (strcmp(_cameras[i]->_name, name) == 0) {
			return (i);
		}
	}
	return -1;
}


void Lib3dsFile::reserveLights(int size, int force) {
	lib3ds_util_reserve_array((void ** *)&_lights, &_nlights, &_lightsSize,
	                          size, force, (Lib3dsFreeFunc)lib3ds_light_free);
}


void Lib3dsFile::insertLight(Lib3dsLight *light, int index) {
	lib3ds_util_insert_array((void ** *)&_lights, &_nlights, &_lightsSize, light, index);
}


void Lib3dsFile::removeLight(int index) {
	lib3ds_util_remove_array((void ** *)&_lights, &_nlights, index, (Lib3dsFreeFunc)lib3ds_light_free);
}


int Lib3dsFile::lightByName(const char *name) {
	for (int i = 0; i < _nlights; ++i) {
		if (strcmp(_lights[i]->name, name) == 0) {
			return (i);
		}
	}
	return -1;
}


void Lib3dsFile::insertMesh(Lib3dsMeshPtr mesh, int index) {
	if (_meshes.size() <= index) {
		_meshes.resize(index + 1);
	}
	_meshes[index] = mesh;
}


void Lib3dsFile::removeMesh(int index) {
	_meshes.remove_at(index);
}


int Lib3dsFile::meshByName(const Common::String &name) {
	for (int i = 0; i < _nmeshes; ++i) {
		if (_meshes[i]->_name == name) {
			return i;
		}
	}
	return -1;
}


Lib3dsMeshPtr Lib3dsFile::meshForNode(Lib3dsNode *node) {
	int index;
	Lib3dsMeshInstanceNode *n;

	if (node->type != LIB3DS_NODE_MESH_INSTANCE)
		return Lib3dsMeshPtr();
	n = (Lib3dsMeshInstanceNode *)node;

	index = meshByName(node->_name);

	return (index >= 0) ? _meshes[index] : Lib3dsMeshPtr();
}


/*!
 * Return a node object by name and type.
 *
 * This function performs a recursive search for the specified node.
 * Both name and type must match.
 *
 * \param file The Lib3dsFile to be searched.
 * \param name The target node name.
 * \param type The target node type
 *
 * \return A pointer to the first matching node, or NULL if not found.
 *
 * \see lib3ds_node_by_name
 */
Lib3dsNode *Lib3dsFile::nodeByName(const Common::String &name, Lib3dsNodeType type) {
	for (Lib3dsNode *p = _nodes; p != 0; p = p->next) {
		if ((p->type == type) && (p->_name == name)) {
			return (p);
		}
		Lib3dsNode *q = lib3ds_node_by_name(p, name, type);
		if (q) {
			return (q);
		}
	}
	return (0);
}


/*!
 * Return a node object by id.
 *
 * This function performs a recursive search for the specified node.
 *
 * \param file The Lib3dsFile to be searched.
 * \param node_id The target node id.
 *
 * \return A pointer to the first matching node, or NULL if not found.
 *
 * \see lib3ds_node_by_id
 */
Lib3dsNode *Lib3dsFile::nodeById(uint16 node_id) {
	Lib3dsNode *p, *q;

	for (p = _nodes; p != 0; p = p->next) {
		if (p->node_id == node_id) {
			return (p);
		}
		q = lib3ds_node_by_id(p, node_id);
		if (q) {
			return (q);
		}
	}
	return (0);
}


void Lib3dsFile::appendNode(Lib3dsNode *node, Lib3dsNode *parent) {
	Lib3dsNode *p;

	assert(node);
	p = parent ? parent->childs : _nodes;
	if (p) {
		while (p->next) {
			p = p->next;
		}
		p->next = node;
	} else {
		if (parent) {
			parent->childs = node;
		} else {
			_nodes = node;
		}
	}
	node->parent = parent;
	node->next = NULL;
}


void Lib3dsFile::insertNode(Lib3dsNode *node, Lib3dsNode *before) {
	Lib3dsNode *p, *q;

	assert(node);

	if (before) {
		p = before->parent ? before->parent->childs : _nodes;
		assert(p);
		q = NULL;
		while (p != before) {
			q = p;
			p = p->next;
		}
		if (q) {
			node->next = q->next;
			q->next = node;
		} else {
			node->next = _nodes;
			_nodes = node;
		}
		node->parent = before->parent;
	} else {
		node->next = _nodes;
		node->parent = NULL;
		_nodes = node;
	}
}


/*!
 * Remove a node from the a Lib3dsFile object.
 *
 * \param file The Lib3dsFile object to be modified.
 * \param node The Lib3dsNode object to be removed from file
 *
 * \return LIB3DS_TRUE on success, LIB3DS_FALSE if node is not found in file
 */
void Lib3dsFile::removeNode(Lib3dsNode *node) {
	Lib3dsNode *p, *n;

	if (node->parent) {
		for (p = 0, n = node->parent->childs; n; p = n, n = n->next) {
			if (n == node) {
				break;
			}
		}
		if (!n) {
			return;
		}

		if (!p) {
			node->parent->childs = n->next;
		} else {
			p->next = n->next;
		}
	} else {
		for (p = 0, n = _nodes; n; p = n, n = n->next) {
			if (n == node) {
				break;
			}
		}
		if (!n) {
			return;
		}

		if (!p) {
			_nodes = n->next;
		} else {
			p->next = n->next;
		}
	}
}


static void file_minmax_node_id_impl(Lib3dsFile *file, Lib3dsNode *node, uint16 *min_id, uint16 *max_id) {
	Lib3dsNode *p;

	if (min_id && (*min_id > node->node_id))
		*min_id = node->node_id;
	if (max_id && (*max_id < node->node_id))
		*max_id = node->node_id;

	p = node->childs;
	while (p) {
		file_minmax_node_id_impl(file, p, min_id, max_id);
		p = p->next;
	}
}


void Lib3dsFile::minmaxNodeId(uint16 *min_id, uint16 *max_id) {
	Lib3dsNode *p;

	if (min_id)
		*min_id = 65535;
	if (max_id)
		*max_id = 0;

	p = _nodes;
	while (p) {
		file_minmax_node_id_impl(this, p, min_id, max_id);
		p = p->next;
	}
}


void Lib3dsFile::boundingBoxOfObjects(int include_meshes, int include_cameras, int include_lights,
										  Math::Vector3d &bmin, Math::Vector3d &bmax) {
	bmin = Math::Vector3d(FLT_MAX, FLT_MAX, FLT_MAX);
	bmax = Math::Vector3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	if (include_meshes) {
		Math::Vector3d lmin, lmax;
		for (int i = 0; i < _nmeshes; ++i) {
			_meshes[i]->boundingBox(lmin, lmax);
			lib3ds_vector_min(bmin, lmin);
			lib3ds_vector_max(bmax, lmax);
		}
	}
	if (include_cameras) {
		for (int i = 0; i < _ncameras; ++i) {
			lib3ds_vector_min(bmin, _cameras[i]->_position);
			lib3ds_vector_max(bmax, _cameras[i]->_position);
			lib3ds_vector_min(bmin, _cameras[i]->_target);
			lib3ds_vector_max(bmax, _cameras[i]->_target);
		}
	}
	if (include_lights) {
		for (int i = 0; i < _ncameras; ++i) {
			lib3ds_vector_min(bmin, _lights[i]->position);
			lib3ds_vector_max(bmax, _lights[i]->position);
			if (_lights[i]->spot_light) {
				lib3ds_vector_min(bmin, _lights[i]->target);
				lib3ds_vector_max(bmax, _lights[i]->target);
			}
		}
	}
}


static void file_bounding_box_of_nodes_impl(Lib3dsNode *node, Lib3dsFile *file,
                                int include_meshes, int include_cameras, int include_lights,
                                Math::Vector3d &bmin, Math::Vector3d &bmax, Math::Matrix4 &matrix) {
	switch (node->type) {
	case LIB3DS_NODE_MESH_INSTANCE:
		if (include_meshes) {
			int index;
			Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;

			index = file->meshByName(n->instance_name);
			if (index < 0)
				index = file->meshByName(node->_name);
			if (index >= 0) {
				Lib3dsMeshPtr mesh = file->_meshes[index];
				Math::Matrix4 inv_matrix = mesh->_matrix;
				inv_matrix.inverse();
				Math::Matrix4 M = matrix * node->matrix;
				lib3ds_matrix_translate(M, -n->pivot.getValue(0), -n->pivot.getValue(1), -n->pivot.getValue(2));
				M = M * inv_matrix;

				Math::Vector3d v;
				for (int i = 0; i < mesh->_nVertices; ++i) {
					lib3ds_vector_transform(v, M, mesh->_vertices[i]);
					lib3ds_vector_min(bmin, v);
					lib3ds_vector_max(bmax, v);
				}
			}
		}
		break;

	case LIB3DS_NODE_CAMERA:
	case LIB3DS_NODE_CAMERA_TARGET:
		if (include_cameras) {
			Math::Matrix4 M;
			M = matrix * node->matrix;
			Math::Vector3d z(0, 0, 0);
			Math::Vector3d v;
			lib3ds_vector_transform(v, M, z);
			lib3ds_vector_min(bmin, v);
			lib3ds_vector_max(bmax, v);
		}
		break;

	case LIB3DS_NODE_OMNILIGHT:
	case LIB3DS_NODE_SPOTLIGHT:
	case LIB3DS_NODE_SPOTLIGHT_TARGET:
		if (include_lights) {
			Math::Matrix4 M = matrix * node->matrix;
			Math::Vector3d z(0, 0, 0);
			Math::Vector3d v;
			lib3ds_vector_transform(v, M, z);
			lib3ds_vector_min(bmin, v);
			lib3ds_vector_max(bmax, v);
		}
		break;
	}
	{
		Lib3dsNode *p = node->childs;
		while (p) {
			file_bounding_box_of_nodes_impl(p, file, include_meshes, include_cameras, include_lights, bmin, bmax, matrix);
			p = p->next;
		}
	}
}


void Lib3dsFile::boundingBoxOfNodes(int include_meshes, int include_cameras, int include_lights,
										Math::Vector3d &bmin, Math::Vector3d &bmax, const Math::Matrix4 &matrix) {
	Lib3dsNode *p;
	Math::Matrix4 M = matrix;

	bmin = Math::Vector3d(FLT_MAX, FLT_MAX, FLT_MAX);
	bmax = Math::Vector3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	p = _nodes;
	while (p) {
		file_bounding_box_of_nodes_impl(p, this, include_meshes, include_cameras, include_lights, bmin, bmax, M);
		p = p->next;
	}
}


void Lib3dsFile::createNodesForMeshes() {
	Lib3dsNode *p;

	for (int i = 0; i < _nmeshes; ++i) {
		Lib3dsMeshPtr mesh = _meshes[i];
		p = lib3ds_node_new(LIB3DS_NODE_MESH_INSTANCE);
		p->_name = mesh->_name;
		insertNode(p, NULL);
	}
}
