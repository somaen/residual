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
#include <stddef.h>

#ifndef LIB3DSAPI
#if defined(_MSC_VER) && !defined(LIB3DS_STATIC)
#ifdef LIB3DS_EXPORTS
#define LIB3DSAPI __declspec(dllexport)
#else
#define LIB3DSAPI __declspec(dllimport)
#endif
#else
#define LIB3DSAPI
#endif
#endif

/** @defgroup api API
    lib3ds public API. */
/** @{ */

enum Lib3dsLogLevel {
	LIB3DS_LOG_ERROR    = 0,
	LIB3DS_LOG_WARN     = 1,
	LIB3DS_LOG_INFO     = 2,
	LIB3DS_LOG_DEBUG    = 3
};

struct Lib3dsIo {
	void   *impl;
	Common::SeekableReadStream *stream;
	void (*log_func)(Common::SeekableReadStream *self, Lib3dsLogLevel level, int indent, const char *msg);
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

enum Lib3dsKeyFlags {
	LIB3DS_KEY_USE_TENS         = 0x01,
	LIB3DS_KEY_USE_CONT         = 0x02,
	LIB3DS_KEY_USE_BIAS         = 0x04,
	LIB3DS_KEY_USE_EASE_TO      = 0x08,
	LIB3DS_KEY_USE_EASE_FROM    = 0x10
};

template<typename T>
struct Lib3dsKey {
	int         frame;
	unsigned    flags;
	float       tens;
	float       cont;
	float       bias;
	float       ease_to;
	float       ease_from;
	T           value;
};

typedef Lib3dsKey<bool> Lib3dsBoolKey;
typedef Lib3dsKey<float> Lib3dsFloatKey;
typedef Lib3dsKey<Math::Vector3d> Lib3dsVectorKey;
typedef Lib3dsKey<Math::Quaternion> Lib3dsQuatKey;

enum Lib3dsTrackType {
	LIB3DS_TRACK_BOOL   = 0,
	LIB3DS_TRACK_FLOAT  = 1,
	LIB3DS_TRACK_VECTOR = 3,
	LIB3DS_TRACK_QUAT   = 4
};

enum {
	LIB3DS_TRACK_REPEAT   = 0x0001,
	LIB3DS_TRACK_SMOOTH   = 0x0002,
	LIB3DS_TRACK_LOCK_X   = 0x0008,
	LIB3DS_TRACK_LOCK_Y   = 0x0010,
	LIB3DS_TRACK_LOCK_Z   = 0x0020,
	LIB3DS_TRACK_UNLINK_X = 0x0100,
	LIB3DS_TRACK_UNLINK_Y = 0x0200,
	LIB3DS_TRACK_UNLINK_Z = 0x0400
};

template<typename T>
struct Lib3dsTrack {
public:
	Lib3dsTrack();
	unsigned        flags;
	int             nkeys;
	void resize(int newSize) {
		keys.resize(newSize);
	}
	Common::Array<T> keys;
	Lib3dsTrackType _type;
};

template<> Lib3dsTrack<Lib3dsBoolKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsBoolKey> Lib3dsBoolTrack;

template<> Lib3dsTrack<Lib3dsFloatKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsFloatKey> Lib3dsFloatTrack;

template<> Lib3dsTrack<Lib3dsVectorKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsVectorKey> Lib3dsVectorTrack;

template<> Lib3dsTrack<Lib3dsQuatKey>::Lib3dsTrack();
typedef Lib3dsTrack<Lib3dsQuatKey> Lib3dsQuatTrack;

struct Lib3dsAmbientColorNode : public Lib3dsNode {
	Math::Vector3d        color;
	Lib3dsVectorTrack     color_track;
};

struct Lib3dsMeshInstanceNode : public Lib3dsNode {
	Math::Vector3d        pivot;
	char                  instance_name[64];
	Math::Vector3d        bbox_min;
	Math::Vector3d        bbox_max;
	int                   hide;
	Math::Vector3d        pos;
	Math::Quaternion      rot;
	Math::Vector3d        scl;
	float                 morph_smooth;
	char                  morph[64];
	Lib3dsVectorTrack     pos_track;
	Lib3dsQuatTrack       rot_track;
	Lib3dsVectorTrack     scl_track;
	Lib3dsBoolTrack       hide_track;
};

struct Lib3dsCameraNode : public Lib3dsNode {
	Math::Vector3d  	  pos;
	float           	  fov;
	float           	  roll;
	Lib3dsVectorTrack     pos_track;
	Lib3dsFloatTrack      fov_track;
	Lib3dsFloatTrack      roll_track;
};

struct Lib3dsTargetNode : public Lib3dsNode  {
	Math::Vector3d        pos;
	Lib3dsVectorTrack     pos_track;
};

struct Lib3dsOmnilightNode : public Lib3dsNode  {
	Math::Vector3d        pos;
	Math::Vector3d        color;
	Lib3dsVectorTrack     pos_track;
	Lib3dsVectorTrack     color_track;
};

		

struct Lib3dsSpotlightNode : public Lib3dsNode  {
	Math::Vector3d        pos;
	Math::Vector3d        color;
	float                 hotspot;
	float                 falloff;
	float                 roll;
	Lib3dsVectorTrack     pos_track;
	Lib3dsVectorTrack     color_track;
	Lib3dsFloatTrack      hotspot_track;
	Lib3dsFloatTrack      falloff_track;
	Lib3dsFloatTrack      roll_track;
};

struct Lib3dsFile {
	unsigned                      _userId;
	void                         *_userPtr;
	unsigned                      _meshVersion;
	unsigned                      _keyfRevision;
	Common::String                _name;
	float                         _masterScale;
	float                         _constructionPlane[3];
	float                         _ambient[3];
	Lib3dsShadow                  _shadow;
	Lib3dsBackground              _background;
	Lib3dsAtmosphere              _atmosphere;
	Lib3dsViewport                _viewport;
	Lib3dsViewport                _viewportKeyf;
	int                           _frames;
	int                           _segmentFrom;
	int                           _segmentTo;
	int                           _currentFrame;
	int                           _materialsSize;
	int                           _nmaterials;
	Lib3dsMaterial              **_materials;
	int                           _camerasSize;
	int                           _ncameras;
	Lib3dsCamera                **_cameras;
	int                           _lightsSize;
	int                           _nlights;
	Lib3dsLight                 **_lights;
	int                           _meshesSize;
	int                           _nmeshes;
	Common::Array<Lib3dsMeshPtr>  _meshes;
	Lib3dsNode                   *_nodes;
	
	Lib3dsFile();
	~Lib3dsFile();
	
	void reserveMaterials(int size, int force);
	void reserveCameras(int size, int force);
	void reserveLights(int size, int force);

	void insertMaterial(Lib3dsMaterial *material, int index);
	void insertCamera(Lib3dsCamera *camera, int index);
	void insertLight(Lib3dsLight *light, int index);
	void insertMesh(Lib3dsMeshPtr mesh, int index);
	void insertNode(Lib3dsNode *node, Lib3dsNode *at);

	void removeMaterial(int index);
	void removeCamera(int index);
	void removeMesh(int index);
	void removeLight(int index);
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

extern LIB3DSAPI Lib3dsFile *lib3ds_file_open(const char *filename);
extern LIB3DSAPI Lib3dsFile *lib3ds_file_open(Common::SeekableReadStream *stream);
extern LIB3DSAPI int lib3ds_file_read(Lib3dsFile *file, Lib3dsIo *io);

extern LIB3DSAPI void lib3ds_material_free(Lib3dsMaterial *material);
extern LIB3DSAPI void lib3ds_light_free(Lib3dsLight *mesh);

extern LIB3DSAPI Lib3dsNode *lib3ds_node_new(Lib3dsNodeType type);
extern LIB3DSAPI Lib3dsAmbientColorNode *lib3ds_node_new_ambient_color(float color0[3]);
extern LIB3DSAPI Lib3dsMeshInstanceNode *lib3ds_node_new_mesh_instance(Lib3dsMesh *mesh, const char *instance_name, float pos0[3], float scl0[3], float rot0[4]);
extern LIB3DSAPI Lib3dsCameraNode *lib3ds_node_new_camera(Lib3dsCamera *camera);
extern LIB3DSAPI Lib3dsTargetNode *lib3ds_node_new_camera_target(Lib3dsCamera *camera);
extern LIB3DSAPI Lib3dsOmnilightNode *lib3ds_node_new_omnilight(Lib3dsLight *light);
extern LIB3DSAPI Lib3dsSpotlightNode *lib3ds_node_new_spotlight(Lib3dsLight *light);
extern LIB3DSAPI Lib3dsTargetNode *lib3ds_node_new_spotlight_target(Lib3dsLight *light);
extern LIB3DSAPI void lib3ds_node_free(Lib3dsNode *node);
extern LIB3DSAPI void lib3ds_node_eval(Lib3dsNode *node, float t);
extern LIB3DSAPI Lib3dsNode *lib3ds_node_by_name(Lib3dsNode *node, const Common::String &name, Lib3dsNodeType type);
extern LIB3DSAPI Lib3dsNode *lib3ds_node_by_id(Lib3dsNode *node, unsigned short node_id);

extern LIB3DSAPI void lib3ds_track_eval_bool(Lib3dsTrack<Lib3dsBoolKey> *track, int *b, float t);
extern LIB3DSAPI void lib3ds_track_eval_float(Lib3dsTrack<Lib3dsFloatKey> *track, float *f, float t);
extern LIB3DSAPI void lib3ds_track_eval_vector(Lib3dsTrack<Lib3dsVectorKey> *track, Math::Vector3d &v, float t);
extern LIB3DSAPI void lib3ds_track_eval_quat(Lib3dsTrack<Lib3dsQuatKey> *track, Math::Quaternion &q, float t);

template<typename T>
void lib3ds_util_reserve_array_delete(T ***ptr, int *n, int *size, int new_size, int force) {
	assert(ptr && n && size);
	if ((*size < new_size) || force) {
		if (force) {
			for (int i = new_size; i < *n; ++i) {
				delete((*ptr)[i]);
				(*ptr)[i] = nullptr;
			}
		}
		*ptr = (T **)realloc(*ptr, sizeof(T *) * new_size);
		*size = new_size;
		if (*n > new_size) {
			*n = new_size;
		}
	}
}

template<typename T>
void lib3ds_util_remove_array_delete(T ***ptr, int *n, int index) {
	assert(ptr && n);
	if ((index >= 0) && (index < *n)) {
		assert(*ptr);
		delete ((*ptr)[index]);

		if (index < *n - 1) {
			memmove(&(*ptr)[index], &(*ptr)[index + 1], sizeof(T *) * (*n - index - 1));
		}
		*n = *n - 1;
	}
}


/** @} */
#endif
