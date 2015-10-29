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
#include "lib3ds_math.h"
#include "lib3ds_node.h"
#include "lib3ds_vector.h"
#include "lib3ds_matrix.h"
#include <string.h>
#include <assert.h>
#include <math.h>

/*!
 * Create and return a new node object.
 *
 * The node is returned with an identity matrix. All other fields
 * are zero.
 *
 * \return Lib3dsNode
 */
Lib3dsNode *lib3ds_node_new(Lib3dsNodeType type) {
	Lib3dsNode *node;
	switch (type) {
	case LIB3DS_NODE_AMBIENT_COLOR: {
		Lib3dsAmbientColorNode *n = new Lib3dsAmbientColorNode;
		node = (Lib3dsNode *)n;
		node->_name = "$AMBIENT$";
		break;
	}

	case LIB3DS_NODE_MESH_INSTANCE: {
		Lib3dsMeshInstanceNode *n = new Lib3dsMeshInstanceNode;
		node = (Lib3dsNode *)n;
		node->_name = "$$$DUMMY";
		break;
	}

	case LIB3DS_NODE_CAMERA: {
		Lib3dsCameraNode *n = new Lib3dsCameraNode;
		node = (Lib3dsNode *)n;
		break;
	}

	case LIB3DS_NODE_CAMERA_TARGET: {
		Lib3dsTargetNode *n = new Lib3dsTargetNode;
		node = (Lib3dsNode *)n;
		break;
	}

	case LIB3DS_NODE_OMNILIGHT: {
		Lib3dsOmnilightNode *n = new Lib3dsOmnilightNode;
		node = (Lib3dsNode *)n;
		break;
	}

	case LIB3DS_NODE_SPOTLIGHT: {
		Lib3dsSpotlightNode *n = new Lib3dsSpotlightNode;
		node = (Lib3dsNode *)n;
		break;
	}

	case LIB3DS_NODE_SPOTLIGHT_TARGET: {
		Lib3dsTargetNode *n = new Lib3dsTargetNode;
		node = (Lib3dsNode *)n;
		break;
	}

	default:
		assert(0);
		return NULL;
	}

	node->type = type;
	node->node_id = 65535;
	node->user_id = 65535;
	lib3ds_matrix_identity(node->matrix);
	return node;
}


Lib3dsAmbientColorNode *lib3ds_node_new_ambient_color(float color0[3]) {
	Lib3dsNode *node = lib3ds_node_new(LIB3DS_NODE_AMBIENT_COLOR);

	Lib3dsAmbientColorNode *n = (Lib3dsAmbientColorNode *)node;
	n->color_track.resize(1);
	if (color0) {
		lib3ds_vector_copy(n->color_track.keys[0].value, color0);
	} else {
		lib3ds_vector_zero(n->color_track.keys[0].value);
	}

	return n;
}


Lib3dsMeshInstanceNode *lib3ds_node_new_mesh_instance(Lib3dsMesh *mesh, const char *instance_name, float pos0[3], float scl0[3], const Math::Quaternion &rot0) {
	Lib3dsMeshInstanceNode *n;

	Lib3dsNode *node = lib3ds_node_new(LIB3DS_NODE_MESH_INSTANCE);
	if (mesh) {
		node->_name = mesh->_name;
	} else {
		node->_name = "$$$DUMMY";
	}

	n = (Lib3dsMeshInstanceNode *)node;
	if (instance_name) {
		strcpy(n->instance_name, instance_name);
	}

	n->pos_track.resize(1);
	if (pos0) {
		lib3ds_vector_copy(n->pos_track.keys[0].value, pos0);
	}

	n->scl_track.resize(1);
	if (scl0) {
		lib3ds_vector_copy(n->scl_track.keys[0].value, scl0);
	} else {
		lib3ds_vector_copy(n->scl_track.keys[0].value, Math::Vector3d(1, 1, 1));
	}

	n->rot_track.resize(1);

	n->rot_track.keys[0].value = rot0;

	return n;
}


Lib3dsCameraNode *lib3ds_node_new_camera(Lib3dsCamera *camera) {
	Lib3dsNode *node = lib3ds_node_new(LIB3DS_NODE_CAMERA);
	Lib3dsCameraNode *n;

	assert(camera);
	node = lib3ds_node_new(LIB3DS_NODE_CAMERA);
	node->_name = camera->_name;

	n = (Lib3dsCameraNode *)node;
	n->pos_track.resize(1);
	lib3ds_vector_copy(n->pos_track.keys[0].value, camera->_position);

	n->fov_track.resize(1);
	n->fov_track.keys[0].value = camera->_fov;

	n->roll_track.resize(1);
	n->roll_track.keys[0].value = camera->_roll;

	return n;
}


Lib3dsTargetNode *lib3ds_node_new_camera_target(Lib3dsCamera *camera) {
	Lib3dsNode *node;
	Lib3dsTargetNode *n;

	assert(camera);
	node = lib3ds_node_new(LIB3DS_NODE_CAMERA_TARGET);
	node->_name = camera->_name;

	n = (Lib3dsTargetNode *)node;
	n->pos_track.resize(1);
	lib3ds_vector_copy(n->pos_track.keys[0].value, camera->_target);

	return n;
}


Lib3dsOmnilightNode *lib3ds_node_new_omnilight(Lib3dsLight *light) {
	Lib3dsNode *node;
	Lib3dsOmnilightNode *n;

	assert(light);
	node = lib3ds_node_new(LIB3DS_NODE_OMNILIGHT);
	node->_name = light->name;

	n = (Lib3dsOmnilightNode *)node;
	n->pos_track.resize(1);
	lib3ds_vector_copy(n->pos_track.keys[0].value, light->position);

	n->color_track.resize(1);
	lib3ds_vector_copy(n->color_track.keys[0].value, light->color);

	return n;
}


Lib3dsSpotlightNode *lib3ds_node_new_spotlight(Lib3dsLight *light) {
	Lib3dsNode *node;
	Lib3dsSpotlightNode *n;

	assert(light);
	node = lib3ds_node_new(LIB3DS_NODE_SPOTLIGHT);
	node->_name = light->name;

	n = (Lib3dsSpotlightNode *)node;
	n->pos_track.resize(1);
	lib3ds_vector_copy(n->pos_track.keys[0].value, light->position);

	n->color_track.resize(1);
	lib3ds_vector_copy(n->color_track.keys[0].value, light->color);

	n->hotspot_track.resize(1);
	n->hotspot_track.keys[0].value = light->hotspot;

	n->falloff_track.resize(1);
	n->falloff_track.keys[0].value = light->falloff;

	n->roll_track.resize(1);
	n->roll_track.keys[0].value = light->roll;

	return n;
}


Lib3dsTargetNode *lib3ds_node_new_spotligf_target(Lib3dsLight *light) {
	Lib3dsNode *node;
	Lib3dsTargetNode *n;

	assert(light);
	node = lib3ds_node_new(LIB3DS_NODE_SPOTLIGHT_TARGET);
	node->_name = light->name;

	n = (Lib3dsTargetNode *)node;
	n->pos_track.resize(1);
	lib3ds_vector_copy(n->pos_track.keys[0].value, light->target);

	return n;
}


static void free_node_and_childs(Lib3dsNode *node) {
	assert(node);
	{
		Lib3dsNode *p, *q;
		for (p = node->childs; p; p = q) {
			q = p->next;
			free_node_and_childs(p);
		}
	}
	delete node;
}


/*!
 * Free a node and all of its resources.
 *
 * \param node Lib3dsNode object to be freed.
 */
void lib3ds_node_free(Lib3dsNode *node) {
	assert(node);
	free_node_and_childs(node);
}


/*!
 * Evaluate an animation node.
 *
 * Recursively sets node and its children to their appropriate values
 * for this point in the animation.
 *
 * \param node Node to be evaluated.
 * \param t time value, between 0. and file->frames
 */
void lib3ds_node_eval(Lib3dsNode *node, float t) {
	assert(node);
	switch (node->type) {
	case LIB3DS_NODE_AMBIENT_COLOR: {
		Lib3dsAmbientColorNode *n = (Lib3dsAmbientColorNode *)node;
		if (node->parent) {
			node->matrix = node->parent->matrix;
		} else {
			lib3ds_matrix_identity(node->matrix);
		}
		lib3ds_track_eval_vector(&n->color_track, n->color, t);
		break;
	}

	case LIB3DS_NODE_MESH_INSTANCE: {
		Math::Matrix4 M;
		Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;

		lib3ds_track_eval_vector(&n->pos_track, n->pos, t);
		lib3ds_track_eval_quat(&n->rot_track, n->rot, t);
		if (n->scl_track.nkeys) {
			lib3ds_track_eval_vector(&n->scl_track, n->scl, t);
		} else {
			n->scl = Math::Vector3d(1, 1, 1);
		}
		lib3ds_track_eval_bool(&n->hide_track, &n->hide, t);

		lib3ds_matrix_identity(M);
		lib3ds_matrix_translate(M, n->pos.getValue(0), n->pos.getValue(1), n->pos.getValue(2));
		lib3ds_matrix_rotate_quat(M, n->rot);
		lib3ds_matrix_scale(M, n->scl.getValue(0), n->scl.getValue(1), n->scl.getValue(2));

		if (node->parent) {
			node->matrix = node->parent->matrix * M;
		} else {
			node->matrix = M;
		}
		break;
	}

	case LIB3DS_NODE_CAMERA: {
		Lib3dsCameraNode *n = (Lib3dsCameraNode *)node;
		lib3ds_track_eval_vector(&n->pos_track, n->pos, t);
		lib3ds_track_eval_float(&n->fov_track, &n->fov, t);
		lib3ds_track_eval_float(&n->roll_track, &n->roll, t);
		if (node->parent) {
			node->matrix = node->parent->matrix;
		} else {
			lib3ds_matrix_identity(node->matrix);
		}
		lib3ds_matrix_translate(node->matrix, n->pos.getValue(0), n->pos.getValue(1), n->pos.getValue(2));
		break;
	}

	case LIB3DS_NODE_CAMERA_TARGET: {
		Lib3dsTargetNode *n = (Lib3dsTargetNode *)node;
		lib3ds_track_eval_vector(&n->pos_track, n->pos, t);
		if (node->parent) {
			node->matrix = node->parent->matrix;
		} else {
			lib3ds_matrix_identity(node->matrix);
		}
		lib3ds_matrix_translate(node->matrix, n->pos.getValue(0), n->pos.getValue(1), n->pos.getValue(2));
		break;
	}

	case LIB3DS_NODE_OMNILIGHT: {
		Lib3dsOmnilightNode *n = (Lib3dsOmnilightNode *)node;
		lib3ds_track_eval_vector(&n->pos_track, n->pos, t);
		lib3ds_track_eval_vector(&n->color_track, n->color, t);
		if (node->parent) {
			node->matrix = node->parent->matrix;
		} else {
			lib3ds_matrix_identity(node->matrix);
		}
		lib3ds_matrix_translate(node->matrix, n->pos.getValue(0), n->pos.getValue(1), n->pos.getValue(2));
		break;
	}

	case LIB3DS_NODE_SPOTLIGHT: {
		Lib3dsSpotlightNode *n = (Lib3dsSpotlightNode *)node;
		lib3ds_track_eval_vector(&n->pos_track, n->pos, t);
		lib3ds_track_eval_vector(&n->color_track, n->color, t);
		lib3ds_track_eval_float(&n->hotspot_track, &n->hotspot, t);
		lib3ds_track_eval_float(&n->falloff_track, &n->falloff, t);
		lib3ds_track_eval_float(&n->roll_track, &n->roll, t);
		if (node->parent) {
			node->matrix = node->parent->matrix;
		} else {
			lib3ds_matrix_identity(node->matrix);
		}
		lib3ds_matrix_translate(node->matrix, n->pos.getValue(0), n->pos.getValue(1), n->pos.getValue(2));
		break;
	}

	case LIB3DS_NODE_SPOTLIGHT_TARGET: {
		Lib3dsTargetNode *n = (Lib3dsTargetNode *)node;
		lib3ds_track_eval_vector(&n->pos_track, n->pos, t);
		if (node->parent) {
			node->matrix = node->parent->matrix;
		} else {
			lib3ds_matrix_identity(node->matrix);
		}
		lib3ds_matrix_translate(node->matrix, n->pos.getValue(0), n->pos.getValue(1), n->pos.getValue(2));
		break;
	}
	}
	{
		Lib3dsNode *p;
		for (p = node->childs; p != 0; p = p->next) {
			lib3ds_node_eval(p, t);
		}
	}
}


/*!
 * Return a node object by name and type.
 *
 * This function performs a recursive search for the specified node.
 * Both name and type must match.
 *
 * \param node The parent node for the search
 * \param name The target node name.
 * \param type The target node type
 *
 * \return A pointer to the first matching node, or NULL if not found.
 */
Lib3dsNode *lib3ds_node_by_name(Lib3dsNode *node, const Common::String &name, Lib3dsNodeType type) {
	Lib3dsNode *p, *q;

	for (p = node->childs; p != 0; p = p->next) {
		if ((p->type == type) && (p->_name == name)) {
			return (p);
		}
		q = lib3ds_node_by_name(p, name, type);
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
 * \param node The parent node for the search
 * \param node_id The target node id.
 *
 * \return A pointer to the first matching node, or NULL if not found.
 */
Lib3dsNode *
lib3ds_node_by_id(Lib3dsNode *node, uint16 node_id) {
	Lib3dsNode *p, *q;

	for (p = node->childs; p != 0; p = p->next) {
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


void lib3ds_node_read(Lib3dsNode *node, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	assert(node);
	lib3ds_chunk_read_start(&c, 0, io);

	switch (c.chunk) {
	case CHK_AMBIENT_NODE_TAG:
	case CHK_OBJECT_NODE_TAG:
	case CHK_CAMERA_NODE_TAG:
	case CHK_TARGET_NODE_TAG:
	case CHK_LIGHT_NODE_TAG:
	case CHK_SPOTLIGHT_NODE_TAG:
	case CHK_L_TARGET_NODE_TAG:
		break;
	default:
		return;
	}

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0)  {
		switch (chunk) {
		case CHK_NODE_ID: {
			node->node_id = stream->readUint16LE();
			lib3ds_io_log_indent(io, 1);
			lib3ds_io_log(io, LIB3DS_LOG_INFO, "ID=%d", (short)node->node_id);
			lib3ds_io_log_indent(io, -1);
			break;
		}

		case CHK_NODE_HDR: {
			lib3ds_io_read_string(io, node->_name, 64);
			node->flags = stream->readUint16LE();
			node->flags |= ((uint32)stream->readUint16LE()) << 16;
			node->user_id = stream->readUint16LE();

			lib3ds_io_log_indent(io, 1);
			lib3ds_io_log(io, LIB3DS_LOG_INFO, "NAME=%s", node->_name.c_str());
			lib3ds_io_log(io, LIB3DS_LOG_INFO, "PARENT=%d", (short)node->user_id);
			lib3ds_io_log_indent(io, -1);
			break;
		}

		case CHK_PIVOT: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				lib3ds_io_read_vector(stream, n->pivot);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_INSTANCE_NAME: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				lib3ds_io_read_string(io, n->instance_name, 64);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_BOUNDBOX: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				lib3ds_io_read_vector(stream, n->bbox_min);
				lib3ds_io_read_vector(stream, n->bbox_max);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_COL_TRACK_TAG: {
			Lib3dsVectorTrack *track = 0;
			switch (node->type) {
			case LIB3DS_NODE_AMBIENT_COLOR: {
				Lib3dsAmbientColorNode *n = (Lib3dsAmbientColorNode *)node;
				track = &n->color_track;
				break;
			}
			case LIB3DS_NODE_OMNILIGHT: {
				Lib3dsOmnilightNode *n = (Lib3dsOmnilightNode *)node;
				track = &n->color_track;
				break;
			}
			case LIB3DS_NODE_SPOTLIGHT: {
				Lib3dsSpotlightNode *n = (Lib3dsSpotlightNode *)node;
				track = &n->color_track;
				break;
			}
			default:
				lib3ds_chunk_unknown(chunk, io);
			}
			if (track) {
				lib3ds_track_read(track, io);
			}
			break;
		}

		case CHK_POS_TRACK_TAG: {
			Lib3dsVectorTrack *track = 0;
			switch (node->type) {
			case LIB3DS_NODE_MESH_INSTANCE: {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				track = &n->pos_track;
				break;
			}
			case LIB3DS_NODE_CAMERA: {
				Lib3dsCameraNode *n = (Lib3dsCameraNode *)node;
				track = &n->pos_track;
				break;
			}
			case LIB3DS_NODE_CAMERA_TARGET: {
				Lib3dsTargetNode *n = (Lib3dsTargetNode *)node;
				track = &n->pos_track;
				break;
			}
			case LIB3DS_NODE_OMNILIGHT: {
				Lib3dsOmnilightNode *n = (Lib3dsOmnilightNode *)node;
				track = &n->pos_track;
				break;
			}
			case LIB3DS_NODE_SPOTLIGHT: {
				Lib3dsSpotlightNode *n = (Lib3dsSpotlightNode *)node;
				track = &n->pos_track;
				break;
			}
			case LIB3DS_NODE_SPOTLIGHT_TARGET: {
				Lib3dsTargetNode *n = (Lib3dsTargetNode *)node;
				track = &n->pos_track;
				break;
			}
			default:
				lib3ds_chunk_unknown(chunk, io);
			}
			if (track) {
				lib3ds_track_read(track, io);
			}
			break;
		}

		case CHK_ROT_TRACK_TAG: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				lib3ds_track_read(&n->rot_track, io);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_SCL_TRACK_TAG: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				lib3ds_track_read(&n->scl_track, io);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_FOV_TRACK_TAG: {
			if (node->type == LIB3DS_NODE_CAMERA) {
				Lib3dsCameraNode *n = (Lib3dsCameraNode *)node;
				lib3ds_track_read(&n->fov_track, io);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_HOT_TRACK_TAG: {
			if (node->type == LIB3DS_NODE_SPOTLIGHT) {
				Lib3dsSpotlightNode *n = (Lib3dsSpotlightNode *)node;
				lib3ds_track_read(&n->hotspot_track, io);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_FALL_TRACK_TAG: {
			if (node->type == LIB3DS_NODE_SPOTLIGHT) {
				Lib3dsSpotlightNode *n = (Lib3dsSpotlightNode *)node;
				lib3ds_track_read(&n->falloff_track, io);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_ROLL_TRACK_TAG: {
			switch (node->type) {
			case LIB3DS_NODE_CAMERA: {
				Lib3dsCameraNode *n = (Lib3dsCameraNode *)node;
				lib3ds_track_read(&n->roll_track, io);
				break;
			}
			case LIB3DS_NODE_SPOTLIGHT: {
				Lib3dsSpotlightNode *n = (Lib3dsSpotlightNode *)node;
				lib3ds_track_read(&n->roll_track, io);
				break;
			}
			default:
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_HIDE_TRACK_TAG: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				lib3ds_track_read(&n->hide_track, io);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
			break;
		}

		case CHK_MORPH_SMOOTH: {
			if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
				Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode *)node;
				n->morph_smooth = lib3ds_io_read_float(stream);
			} else {
				lib3ds_chunk_unknown(chunk, io);
			}
		}
		break;

		/*
		case LIB3DS_MORPH_TRACK_TAG: {
		    if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
		        Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode*)node;
		        n->morph_track = lib3ds_track_new(node, LIB3DS_TRACK_MORPH, 0);
		        lib3ds_track_read(n->morph_track, io);
		    } else {
		        lib3ds_chunk_unknown(chunk, io);
		    }
		}
		break;
		*/

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}

