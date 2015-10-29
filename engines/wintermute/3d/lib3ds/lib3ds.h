/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_H
#define INCLUDED_LIB3DS_H
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

/** @file lib3ds.h
    Header file for public API defined by lib3ds */
#include "common/stream.h"
#include "math/matrix4.h"
#include "math/vector2d.h"
#include "math/quat.h"
#include "common/array.h"
#include "common/ptr.h"

/** @defgroup api API
    lib3ds public API. */
/** @{ */

enum Lib3dsLogLevel {
	LIB3DS_LOG_ERROR    = 0,
	LIB3DS_LOG_WARN     = 1,
	LIB3DS_LOG_INFO     = 2,
	LIB3DS_LOG_DEBUG    = 3
};

/* Atmosphere settings */
struct Lib3dsAtmosphere {
	int         use_fog;
	float       fog_color[3];
	int         fog_background;
	float       fog_near_plane;
	float       fog_near_density;
	float       fog_far_plane;
	float       fog_far_density;
	int         use_layer_fog;
	unsigned    layer_fog_flags;
	float       layer_fog_color[3];
	float       layer_fog_near_y;
	float       layer_fog_far_y;
	float       layer_fog_density;
	int         use_dist_cue;
	int         dist_cue_background;     /* bool */
	float       dist_cue_near_plane;
	float       dist_cue_near_dimming;
	float       dist_cue_far_plane;
	float       dist_cue_far_dimming;
};

/* Background settings */
struct Lib3dsBackground {
	int         use_bitmap;
	char        bitmap_name[64];
	int         use_solid;
	float       solid_color[3];
	int         use_gradient;
	float       gradient_percent;
	float       gradient_top[3];
	float       gradient_middle[3];
	float       gradient_bottom[3];
};

/** Shadow settings */
struct Lib3dsShadow {
	short       map_size;           /**< Global shadow map size that ranges from 10 to 4096 */
	float       low_bias;           /**< Global shadow low bias */
	float       hi_bias;            /**< Global shadow hi bias */
	float       filter;             /**< Global shadow filter that ranges from 1 (lowest) to 10 (highest) */
	float       ray_bias;           /**< Global raytraced shadow bias */
};

/* Layout view types */
enum Lib3dsViewType {
	LIB3DS_VIEW_NOT_USED   = 0,
	LIB3DS_VIEW_TOP        = 1,
	LIB3DS_VIEW_BOTTOM     = 2,
	LIB3DS_VIEW_LEFT       = 3,
	LIB3DS_VIEW_RIGHT      = 4,
	LIB3DS_VIEW_FRONT      = 5,
	LIB3DS_VIEW_BACK       = 6,
	LIB3DS_VIEW_USER       = 7,
	LIB3DS_VIEW_SPOTLIGHT  = 18,
	LIB3DS_VIEW_CAMERA     = 65535
};

/* Layout styles */
enum Lib3dsLayoutStyle {
	LIB3DS_LAYOUT_SINGLE                    = 0,
	LIB3DS_LAYOUT_TWO_PANE_VERT_SPLIT       = 1,
	LIB3DS_LAYOUT_TWO_PANE_HORIZ_SPLIT      = 2,
	LIB3DS_LAYOUT_FOUR_PANE                 = 3,
	LIB3DS_LAYOUT_THREE_PANE_LEFT_SPLIT     = 4,
	LIB3DS_LAYOUT_THREE_PANE_BOTTOM_SPLIT   = 5,
	LIB3DS_LAYOUT_THREE_PANE_RIGHT_SPLIT    = 6,
	LIB3DS_LAYOUT_THREE_PANE_TOP_SPLIT      = 7,
	LIB3DS_LAYOUT_THREE_PANE_VERT_SPLIT     = 8,
	LIB3DS_LAYOUT_THREE_PANE_HORIZ_SPLIT    = 9,
	LIB3DS_LAYOUT_FOUR_PANE_LEFT_SPLIT      = 10,
	LIB3DS_LAYOUT_FOUR_PANE_RIGHT_SPLIT     = 11
};

/* Layout view settings */
struct Lib3dsView {
	int         type;
	unsigned    axis_lock;
	short       position[2];
	short       size[2];
	float       zoom;
	Math::Vector3d center;
	float       horiz_angle;
	float       vert_angle;
	char        camera[11];
};

#define LIB3DS_LAYOUT_MAX_VIEWS 32

/* Viewport and default view settings */
struct Lib3dsViewport {
	int             layout_style;
	int             layout_active;
	int             layout_swap;
	int             layout_swap_prior;
	int             layout_swap_view;
	unsigned short  layout_position[2];
	unsigned short  layout_size[2];
	int             layout_nviews;
	Lib3dsView      layout_views[LIB3DS_LAYOUT_MAX_VIEWS];
	int             default_type;
	Math::Vector3d  default_position;
	float           default_width;
	float           default_horiz_angle;
	float           default_vert_angle;
	float           default_roll_angle;
	char            default_camera[64];
};

/* Material texture map flags */
enum Lib3dsTextureMapFlags {
	LIB3DS_TEXTURE_DECALE       = 0x0001,
	LIB3DS_TEXTURE_MIRROR       = 0x0002,
	LIB3DS_TEXTURE_NEGATE       = 0x0008,
	LIB3DS_TEXTURE_NO_TILE      = 0x0010,
	LIB3DS_TEXTURE_SUMMED_AREA  = 0x0020,
	LIB3DS_TEXTURE_ALPHA_SOURCE = 0x0040,
	LIB3DS_TEXTURE_TINT         = 0x0080,
	LIB3DS_TEXTURE_IGNORE_ALPHA = 0x0100,
	LIB3DS_TEXTURE_RGB_TINT     = 0x0200
};

/* Material texture map */
struct Lib3dsTextureMap {
	unsigned    user_id;
	void       *user_ptr;
	char        name[64];
	unsigned    flags;
	float       percent;
	float       blur;
	float       scale[2];
	float       offset[2];
	float       rotation;
	float       tint_1[3];
	float       tint_2[3];
	float       tint_r[3];
	float       tint_g[3];
	float       tint_b[3];
};

/* Auto reflection texture map flags */
enum Lib3dsAutoReflMapFlags {
	LIB3DS_AUTOREFL_USE                     = 0x0001,
	LIB3DS_AUTOREFL_READ_FIRST_FRAME_ONLY   = 0x0004,
	LIB3DS_AUTOREFL_FLAT_MIRROR             = 0x0008
};

/* Material shading type */
enum Lib3dsShading {
	LIB3DS_SHADING_WIRE_FRAME = 0,
	LIB3DS_SHADING_FLAT       = 1,
	LIB3DS_SHADING_GOURAUD    = 2,
	LIB3DS_SHADING_PHONG      = 3,
	LIB3DS_SHADING_METAL      = 4
};

/** Material */
struct Lib3dsMaterial {
	Lib3dsMaterial(const char *name);
	uint32              user_id;
	void               *user_ptr;
	char                name[64];           /* Material name */
	float               ambient[3];         /* Material ambient reflectivity */
	float               diffuse[3];         /* Material diffuse reflectivity */
	float               specular[3];        /* Material specular reflectivity */
	float               shininess;          /* Material specular exponent */
	float               shin_strength;
	int32               use_blur;
	float               blur;
	float               transparency;
	float               falloff;
	int32               is_additive;
	int32               self_illum_flag; /* bool */
	float               self_illum;
	int32               use_falloff;
	int32               shading;
	int32               soften;         /* bool */
	int32               face_map;       /* bool */
	int32               two_sided;      /* Material visible from back */
	int32               map_decal;      /* bool */
	int32               use_wire;
	int32               use_wire_abs;
	float               wire_size;
	Lib3dsTextureMap    texture1_map;
	Lib3dsTextureMap    texture1_mask;
	Lib3dsTextureMap    texture2_map;
	Lib3dsTextureMap    texture2_mask;
	Lib3dsTextureMap    opacity_map;
	Lib3dsTextureMap    opacity_mask;
	Lib3dsTextureMap    bump_map;
	Lib3dsTextureMap    bump_mask;
	Lib3dsTextureMap    specular_map;
	Lib3dsTextureMap    specular_mask;
	Lib3dsTextureMap    shininess_map;
	Lib3dsTextureMap    shininess_mask;
	Lib3dsTextureMap    self_illum_map;
	Lib3dsTextureMap    self_illum_mask;
	Lib3dsTextureMap    reflection_map;
	Lib3dsTextureMap    reflection_mask;
	uint32              autorefl_map_flags;
	int32               autorefl_map_anti_alias;  /* 0=None, 1=Low, 2=Medium, 3=High */
	int32               autorefl_map_size;
	int32               autorefl_map_frame_step;
};

typedef Common::SharedPtr<Lib3dsMaterial> Lib3dsMaterialPtr;

/** Object flags for cameras, lights and meshes */
enum Lib3dsObjectFlags {
	LIB3DS_OBJECT_HIDDEN          = 0x01,
	LIB3DS_OBJECT_VIS_LOFTER      = 0x02,
	LIB3DS_OBJECT_DOESNT_CAST     = 0x04,
	LIB3DS_OBJECT_MATTE           = 0x08,
	LIB3DS_OBJECT_DONT_RCVSHADOW  = 0x10,
	LIB3DS_OBJECT_FAST            = 0x20,
	LIB3DS_OBJECT_FROZEN          = 0x40
};

/** Camera object */
struct Lib3dsCamera {
	unsigned    _userId;
	void       *_userPtr;
	char        _name[64];
	unsigned    _objectFlags; /*< @see Lib3dsObjectFlags */
	float       _position[3];
	float       _target[3];
	float       _roll;
	float       _fov;
	int         _seeCone;
	float       _nearRange;
	float       _farRange;
	
	Lib3dsCamera(const char *name);
	~Lib3dsCamera();
};

typedef Common::SharedPtr<Lib3dsCamera> Lib3dsCameraPtr;

/** Light object */
struct Lib3dsLight {
	Lib3dsLight(const char *name);
	unsigned       user_id;
	void          *user_ptr;
	char           name[64];
	unsigned       object_flags;
	int            spot_light;     /* bool */
	int            see_cone;
	float          color[3];
	float          position[3];
	Math::Vector3d target;
	float          roll;
	int            off;              /* bool */
	float          outer_range;
	float          inner_range;
	float          multiplier;
	/*const char**  excludes;*/
	float          attenuation;
	int            rectangular_spot;   /* bool */
	int            shadowed;           /* bool */
	float          shadow_bias;
	float          shadow_filter;
	int            shadow_size;
	float          spot_aspect;
	int            use_projector;
	char           projector[64];
	int            spot_overshoot;      /* bool */
	int            ray_shadows;         /* bool */
	float          ray_bias;
	float          hotspot;
	float          falloff;
};

typedef Common::SharedPtr<Lib3dsLight> Lib3dsLightPtr;

/* Texture map projection */
enum {
	LIB3DS_MAP_NONE           = -1,
	LIB3DS_MAP_PLANAR         = 0,
	LIB3DS_MAP_CYLINDRICAL    = 1,
	LIB3DS_MAP_SPHERICAL      = 2
};

/**  Meaning of Lib3dsFace::flags. ABC are points of the current face
    (A: is 1st vertex, B is 2nd vertex, C is 3rd vertex) */
enum Lib3dsFaceFlags {
	LIB3DS_FACE_VIS_AC    = 0x01,       /**< Bit 0: Edge visibility AC */
	LIB3DS_FACE_VIS_BC    = 0x02,       /**< Bit 1: Edge visibility BC */
	LIB3DS_FACE_VIS_AB    = 0x04,       /**< Bit 2: Edge visibility AB */
	LIB3DS_FACE_WRAP_U    = 0x08,       /**< Bit 3: Face is at tex U wrap seam */
	LIB3DS_FACE_WRAP_V    = 0x10,       /**< Bit 4: Face is at tex V wrap seam */
	LIB3DS_FACE_SELECT_3  = (1 << 13),  /**< Bit 13: Selection of the face in selection 3*/
	LIB3DS_FACE_SELECT_2  = (1 << 14),  /**< Bit 14: Selection of the face in selection 2*/
	LIB3DS_FACE_SELECT_1  = (1 << 15)  /**< Bit 15: Selection of the face in selection 1*/
};

struct Lib3dsFace {
	unsigned short  index[3];
	unsigned short  flags;
	int             material;
	unsigned        smoothing_group;
};

/* Triangular mesh object */
struct Lib3dsMesh {
	unsigned                       _userId;
	Common::String                 _name;            /**< Mesh name. Don't use more than 8 characters  */
	unsigned                       _objectFlags;        /**< @see Lib3dsObjectFlags */
	int                            _color;               /**< Index to editor palette [0..255] */
	Math::Matrix4                  _matrix;        /**< Transformation matrix for mesh data */
	unsigned short                 _nVertices;           /**< Number of vertices in vertex array (max. 65535) */
	Common::Array<Math::Vector3d>  _vertices;
	Common::Array<Math::Vector2d>  _texCos;
	// TODO: This is allocated in 2 * sizeof(float)-fashion
	Common::Array<uint16>          _vFlags;
	unsigned short                 _nFaces;              /**< Number of faces in face array (max. 65535) */
	Common::Array<Lib3dsFace>      _faces;
	char                           _boxFront[64];
	char                           _boxBack[64];
	char                           _boxLeft[64];
	char                           _boxRight[64];
	char                           _boxTop[64];
	char                           _boxBottom[64];
	int                            _mapType;
	float                          _mapPos[3];
	Math::Matrix4                  _mapMatrix;
	float                          _mapScale;
	float                          _mapTile[2];
	float                          _mapPlanarSize[2];
	float                          _mapCylinderHeight;
	
	Lib3dsMesh(const Common::String &name);
	~Lib3dsMesh();
	void resizeVertices(int nvertices, int use_texcos, int use_flags);
	void resizeFaces(int nfaces);
	void boundingBox(Math::Vector3d &bmin, Math::Vector3d &bmax);
	Common::Array<Math::Vector3d> calculateFaceNormals();
	Common::Array<Math::Vector3d> calculateVertexNormals();
};

typedef Common::SharedPtr<Lib3dsMesh> Lib3dsMeshPtr;

enum Lib3dsNodeType {
	LIB3DS_NODE_AMBIENT_COLOR   = 0,
	LIB3DS_NODE_MESH_INSTANCE   = 1,
	LIB3DS_NODE_CAMERA          = 2,
	LIB3DS_NODE_CAMERA_TARGET   = 3,
	LIB3DS_NODE_OMNILIGHT       = 4,
	LIB3DS_NODE_SPOTLIGHT       = 5,
	LIB3DS_NODE_SPOTLIGHT_TARGET = 6
};

enum Lib3dsNodeFlags {
	LIB3DS_NODE_HIDDEN           = 0x000800,
	LIB3DS_NODE_SHOW_PATH        = 0x010000,
	LIB3DS_NODE_SMOOTHING        = 0x020000,
	LIB3DS_NODE_MOTION_BLUR      = 0x100000,
	LIB3DS_NODE_MORPH_MATERIALS  = 0x400000
};

struct Lib3dsNode {
	unsigned            user_id;
	void               *user_ptr;
	struct Lib3dsNode  *next;
	struct Lib3dsNode  *childs;
	struct Lib3dsNode  *parent;
	Lib3dsNodeType      type;
	unsigned short      node_id;            /**< 0..65535 */
	Common::String      _name;
	unsigned            flags;
	Math::Matrix4       matrix;
};

struct Lib3dsFile {
	unsigned                           _userId;
	void                              *_userPtr;
	unsigned                           _meshVersion;
	unsigned                           _keyfRevision;
	Common::String                     _name;
	float                              _masterScale;
	float                              _constructionPlane[3];
	float                              _ambient[3];
	Lib3dsShadow                       _shadow;
	Lib3dsBackground                   _background;
	Lib3dsAtmosphere                   _atmosphere;
	Lib3dsViewport                     _viewport;
	Lib3dsViewport                     _viewportKeyf;
	int                                _frames;
	int                                _segmentFrom;
	int                                _segmentTo;
	int                                _currentFrame;
	Common::Array<Lib3dsMaterialPtr>   _materials;
	Common::Array<Lib3dsCameraPtr>     _cameras;
	Common::Array<Lib3dsLightPtr>      _lights;
	Common::Array<Lib3dsMeshPtr>       _meshes;
	Lib3dsNode                        *_nodes;
	
	Lib3dsFile();
	~Lib3dsFile();

	void pushMaterial(Lib3dsMaterialPtr material);
	void pushCamera(Lib3dsCameraPtr camera);
	void pushLight(Lib3dsLightPtr light);
	void pushMesh(Lib3dsMeshPtr mesh);
	void insertNode(Lib3dsNode *node, Lib3dsNode *at);

	void removeNode(Lib3dsNode *node);

	int materialByName(const char *name);
	int cameraByName(const char *name);
	int lightByName(const char *name);
	int meshByName(const Common::String &name);
	Lib3dsNode *nodeByName(const Common::String &name, Lib3dsNodeType type);

	Lib3dsMeshPtr meshForNode(Lib3dsNode *node);
	Lib3dsNode *nodeById(unsigned short node_id);
	void appendNode(Lib3dsNode *node, Lib3dsNode *parent);
	void minmaxNodeId(unsigned short *min_id, unsigned short *max_id);
	void createNodesForMeshes();
	/**
	    This function computes the bounding box of meshes, cameras
	    and lights defined in the 3D editor.
	
	    \param include_meshes   Include meshes in bounding box calculation.
	    \param include_cameras  Include cameras in bounding box calculation.
	    \param include_lights   Include lights in bounding box calculation.
	    \param bmin             Returned minimum x,y,z values.
	    \param bmax             Returned maximum x,y,z values.
	 */
	void boundingBoxOfObjects(
    	int include_meshes,
    	int include_cameras,
    	int include_lights,
    	Math::Vector3d &bmin,
		Math::Vector3d &bmax);

	/**
	    This function computes the bounding box of mesh, camera
	    and light instances defined in the Keyframer.
	
	    \param include_meshes   Include meshes in bounding box calculation.
	    \param include_cameras  Include cameras in bounding box calculation.
	    \param include_lights   Include lights in bounding box calculation.
	    \param bmin             Returned minimum x,y,z values.
	    \param bmax             Returned maximum x,y,z values.
	    \param matrix           A matrix describing the coordinate system to
	                            calculate the bounding box in.
	 */
	void boundingBoxOfNodes(
    	int include_meshes,
    	int include_cameras,
    	int include_lights,
		Math::Vector3d &bmin,
		Math::Vector3d &bmax, 
    	const Math::Matrix4 &matrix);

};

Lib3dsFile *lib3ds_file_open(Common::SeekableReadStream *stream);

/** @} */
#endif
