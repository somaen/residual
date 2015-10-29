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
#include "lib3ds_quat.h"
#include "lib3ds_matrix.h"
#include "lib3ds_vector.h"
#include <float.h>
#include <assert.h>
#include <math.h>

/*!
 * Construct new empty mesh object.
 *
 * Mesh is initialized with the name and an identity matrix; all
 * other fields are zero.
 *
 * See Lib3dsFaceFlag for definitions of per-face flags.
 *
 */
Lib3dsMesh::Lib3dsMesh(const Common::String &name) {
	_name = name;
	lib3ds_matrix_identity(_matrix);
	_mapType = LIB3DS_MAP_NONE;
}


/*!
 * Free a mesh object and all of its resources.
 */
Lib3dsMesh::~Lib3dsMesh() {
	resizeVertices(0, 0, 0);
	resizeFaces(0);
}


void Lib3dsMesh::resizeVertices(int nvertices, int use_texcos, int use_flags) {
	assert(nvertices >= _vertices.size());
	_vertices.resize(nvertices);
	_texCos.resize(use_texcos ? nvertices : 0);
	_vFlags.resize(use_flags ? nvertices : 0);
	_nVertices = (unsigned short)nvertices;
}


void Lib3dsMesh::resizeFaces(int nfaces) {
	_faces.resize(nfaces);
	for (int i = _nFaces; i < nfaces; ++i) {
		_faces[i].material = -1;
	}
	_nFaces = (unsigned short)nfaces;
}


/*!
 * Find the bounding box of a mesh object.
 *
 * \param bmin Returned bounding box
 * \param bmax Returned bounding box
 */
void Lib3dsMesh::boundingBox(Math::Vector3d &bmin, Math::Vector3d &bmax) {
	bmin = Math::Vector3d(FLT_MAX, FLT_MAX, FLT_MAX);
	bmax = Math::Vector3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < _nVertices; ++i) {
		lib3ds_vector_min(bmin, _vertices[i]);
		lib3ds_vector_max(bmax, _vertices[i]);
	}
}

Common::Array<Math::Vector3d> Lib3dsMesh::calculateFaceNormals() {
	if (!_nFaces) {
		return Common::Array<Math::Vector3d>();
	}
	Common::Array<Math::Vector3d> faceNormals;
	faceNormals.resize(_nFaces);
	for (int i = 0; i < _nFaces; ++i) {
		lib3ds_vector_normal(
		    faceNormals[i],
		    _vertices[_faces[i].index[0]],
		    _vertices[_faces[i].index[1]],
		    _vertices[_faces[i].index[2]]
		);
	}
	return faceNormals;
}


typedef struct Lib3dsFaces {
	struct Lib3dsFaces *next;
	int index;
	Math::Vector3d normal;
} Lib3dsFaces;


/*!
 * Calculates the vertex normals corresponding to the smoothing group
 * settings for each face of a mesh.
 *
 * \return          A Common::Array containing the normals.
 *                  normals. The buffer must have the size:
 *                  3*3*sizeof(float)*mesh->nfaces.
 *
 * To allocate the normal buffer do for example the following:
 * \code
 *  Lib3dsVector *normals = malloc(3*3*sizeof(float)*mesh->nfaces);
 * \endcode
 *
 * To access the normal of the i-th vertex of the j-th face do the
 * following:
 * \code
 *   normals[3*j+i]
 * \endcode
 */
Common::Array<Math::Vector3d> Lib3dsMesh::calculateVertexNormals() {
	Lib3dsFaces **fl;
	Lib3dsFaces *fa;

	if (!_nFaces) {
		return Common::Array<Math::Vector3d>();
	}

	fl = (Lib3dsFaces **)calloc(sizeof(Lib3dsFaces *), _nVertices);
	fa = (Lib3dsFaces *)malloc(sizeof(Lib3dsFaces) * 3 * _nFaces);

	for (int i = 0; i < _nFaces; ++i) {
		for (int j = 0; j < 3; ++j) {
			Lib3dsFaces *l = &fa[3 * i + j];
			Math::Vector3d p, q, n;
			float len, weight;

			l->index = i;
			l->next = fl[_faces[i].index[j]];
			fl[_faces[i].index[j]] = l;

			p = _vertices[_faces[i].index[j < 2 ? j + 1 : 0]] - _vertices[_faces[i].index[j]];
			q = _vertices[_faces[i].index[j > 0 ? j - 1 : 2]] - _vertices[_faces[i].index[j]];
			lib3ds_vector_cross(n, p, q);
			len = lib3ds_vector_length(n);
			if (len > 0) {
				weight = (float)atan2(len, p.dotProduct(q));
				l->normal =  n * (weight / len);
			} else {
				lib3ds_vector_zero(l->normal);
			}
		}
	}
	
	Common::Array<Math::Vector3d> normals;
	normals.resize(3 * _nFaces);
	for (int i = 0; i < _nFaces; ++i) {
		Lib3dsFace *f = &_faces[i];
		for (int j = 0; j < 3; ++j) {
			Math::Vector3d n;
			Lib3dsFaces *p;
			Lib3dsFace *pf;

			assert(_faces[i].index[j] < _nVertices);

			if (f->smoothing_group) {
				unsigned smoothing_group = f->smoothing_group;

				lib3ds_vector_zero(n);
				for (p = fl[_faces[i].index[j]]; p; p = p->next) {
					pf = &_faces[p->index];
					if (pf->smoothing_group & f->smoothing_group)
						smoothing_group |= pf->smoothing_group;
				}

				for (p = fl[_faces[i].index[j]]; p; p = p->next) {
					pf = &_faces[p->index];
					if (smoothing_group & pf->smoothing_group) {
						n = n + p->normal;
					}
				}
			} else {
				lib3ds_vector_copy(n, fa[3 * i + j].normal);
			}

			n.normalize();
			lib3ds_vector_copy(normals[i + j], n);
		}
	}

	free(fa);
	free(fl);
	
	return normals;
}


static void face_array_read(Lib3dsFile *file, Lib3dsMeshPtr mesh, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;
	uint16 nfaces;
	
	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_FACE_ARRAY, io);

	mesh->resizeFaces(0);
	nfaces = stream->readUint16LE();
	if (nfaces) {
		mesh->resizeFaces(nfaces);
		for (int i = 0; i < nfaces; ++i) {
			mesh->_faces[i].index[0] = stream->readUint16LE();
			mesh->_faces[i].index[1] = stream->readUint16LE();
			mesh->_faces[i].index[2] = stream->readUint16LE();
			mesh->_faces[i].flags = stream->readUint16LE();
		}
		lib3ds_chunk_read_tell(&c, io);

		while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
			switch (chunk) {
			case CHK_MSH_MAT_GROUP: {
				char name[64];
				unsigned n;
				int index;
				int material;

				lib3ds_io_read_string(io, name, 64);
				material = file->materialByName(name);

				n = stream->readUint16LE();
				for (unsigned i = 0; i < n; ++i) {
					index = stream->readUint16LE();
					if (index < mesh->_nFaces) {
						mesh->_faces[index].material = material;
					} else {
						// TODO warning
					}
				}
				break;
			}

			case CHK_SMOOTH_GROUP: {
				for (int i = 0; i < mesh->_nFaces; ++i) {
					mesh->_faces[i].smoothing_group = stream->readUint32LE();
				}
				break;
			}

			case CHK_MSH_BOXMAP: {
				lib3ds_io_read_string(io, mesh->_boxFront, 64);
				lib3ds_io_read_string(io, mesh->_boxBack, 64);
				lib3ds_io_read_string(io, mesh->_boxLeft, 64);
				lib3ds_io_read_string(io, mesh->_boxRight, 64);
				lib3ds_io_read_string(io, mesh->_boxTop, 64);
				lib3ds_io_read_string(io, mesh->_boxBottom, 64);
				break;
			}

			default:
				lib3ds_chunk_unknown(chunk, io);
			}
		}

	}
	lib3ds_chunk_read_end(&c, io);
}


void lib3ds_mesh_read(Lib3dsFile *file, Lib3dsMeshPtr mesh, Lib3dsIo *io) {
	Lib3dsChunk c;
	uint16 chunk;

	Common::SeekableReadStream *stream = io->stream;

	lib3ds_chunk_read_start(&c, CHK_N_TRI_OBJECT, io);

	while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
		switch (chunk) {
		case CHK_MESH_MATRIX: {
			lib3ds_matrix_identity(mesh->_matrix);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					mesh->_matrix.setValue(i, j, lib3ds_io_read_float(stream));
				}
			}
			break;
		}

		case CHK_MESH_COLOR: {
			mesh->_color = stream->readByte();
			break;
		}

		case CHK_POINT_ARRAY: {
			uint16 nvertices = stream->readUint16LE();
			mesh->resizeVertices(nvertices, mesh->_texCos.size() != 0, mesh->_vFlags.size() != 0);
			
			for (int i = 0; i < mesh->_nVertices; ++i) {
				lib3ds_io_read_vector(stream, mesh->_vertices[i]);
			}
			break;
		}

		case CHK_POINT_FLAG_ARRAY: {
			uint16 nflags = stream->readUint16LE();
			uint16 nvertices = (mesh->_nVertices >= nflags) ? mesh->_nVertices : nflags;
			mesh->resizeVertices(nvertices, mesh->_texCos.size() != 0, true);
			for (int i = 0; i < nflags; ++i) {
				mesh->_vFlags[i] = stream->readUint16LE();
			}
			break;
		}

		case CHK_FACE_ARRAY: {
			lib3ds_chunk_read_reset(&c, io);
			face_array_read(file, mesh, io);
			break;
		}

		case CHK_MESH_TEXTURE_INFO: {
			//FIXME: mesh->map_type = lib3ds_io_read_word(io);

			for (int i = 0; i < 2; ++i) {
				mesh->_mapTile[i] = lib3ds_io_read_float(stream);
			}
			for (int i = 0; i < 3; ++i) {
				mesh->_mapPos[i] = lib3ds_io_read_float(stream);
			}
			mesh->_mapScale = lib3ds_io_read_float(stream);

			lib3ds_matrix_identity(mesh->_mapMatrix);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					mesh->_mapMatrix.setValue(i, j, lib3ds_io_read_float(stream));
				}
			}
			for (int i = 0; i < 2; ++i) {
				mesh->_mapPlanarSize[i] = lib3ds_io_read_float(stream);
			}
			mesh->_mapCylinderHeight = lib3ds_io_read_float(stream);
			break;
		}

		case CHK_TEX_VERTS: {
			uint16 ntexcos = stream->readUint16LE();
			uint16 nvertices = (mesh->_nVertices >= ntexcos) ? mesh->_nVertices : ntexcos;;
			if (mesh->_texCos.size() == 0) {
				mesh->resizeVertices(nvertices, true, mesh->_vFlags.size() != 0);
			}
			for (int i = 0; i < ntexcos; ++i) {
				mesh->_texCos[i].setValue(0, lib3ds_io_read_float(stream));
				mesh->_texCos[i].setValue(1, lib3ds_io_read_float(stream));
			}
			break;
		}

		default:
			lib3ds_chunk_unknown(chunk, io);
		}
	}

	if (lib3ds_matrix_det(mesh->_matrix) < 0.0) {
		/* Flip X coordinate of vertices if mesh matrix
		   has negative determinant */
		Math::Matrix4 inv_matrix = mesh->_matrix;
		inv_matrix.inverse();

		Math::Matrix4 M = mesh->_matrix;
		lib3ds_matrix_scale(M, -1.0f, 1.0f, 1.0f);
		M = M * inv_matrix;

		for (int i = 0; i < mesh->_nVertices; ++i) {
			Math::Vector3d tmp;
			lib3ds_vector_transform(tmp, M, mesh->_vertices[i]);
			lib3ds_vector_copy(mesh->_vertices[i], tmp);
		}
	}

	lib3ds_chunk_read_end(&c, io);
}

