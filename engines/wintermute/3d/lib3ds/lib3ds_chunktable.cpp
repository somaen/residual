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


typedef struct Lib3dsChunkTable {
	uint32_t chunk;
	const char *name;
} Lib3dsChunkTable;


static Lib3dsChunkTable lib3ds_chunk_table[] = {
	{CHK_NULL_CHUNK, "NULL_CHUNK"},
	{CHK_M3DMAGIC, "M3DMAGIC"},
	{CHK_SMAGIC, "SMAGIC"},
	{CHK_LMAGIC, "LMAGIC"},
	{CHK_MLIBMAGIC, "MLIBMAGIC"},
	{CHK_MATMAGIC, "MATMAGIC"},
	{CHK_CMAGIC, "CMAGIC"},
	{CHK_M3D_VERSION, "M3D_VERSION"},
	{CHK_M3D_KFVERSION, "M3D_KFVERSION"},
	{CHK_COLOR_F, "COLOR_F"},
	{CHK_COLOR_24, "COLOR_24"},
	{CHK_LIN_COLOR_24, "LIN_COLOR_24"},
	{CHK_LIN_COLOR_F, "LIN_COLOR_F"},
	{CHK_INT_PERCENTAGE, "INT_PERCENTAGE"},
	{CHK_FLOAT_PERCENTAGE, "FLOAT_PERCENTAGE"},
	{CHK_MDATA, "MDATA"},
	{CHK_MESH_VERSION, "MESH_VERSION"},
	{CHK_MASTER_SCALE, "MASTER_SCALE"},
	{CHK_LO_SHADOW_BIAS, "LO_SHADOW_BIAS"},
	{CHK_HI_SHADOW_BIAS, "HI_SHADOW_BIAS"},
	{CHK_SHADOW_MAP_SIZE, "SHADOW_MAP_SIZE"},
	{CHK_SHADOW_SAMPLES, "SHADOW_SAMPLES"},
	{CHK_SHADOW_RANGE, "SHADOW_RANGE"},
	{CHK_SHADOW_FILTER, "SHADOW_FILTER"},
	{CHK_RAY_BIAS, "RAY_BIAS"},
	{CHK_O_CONSTS, "O_CONSTS"},
	{CHK_AMBIENT_LIGHT, "AMBIENT_LIGHT"},
	{CHK_BIT_MAP, "BIT_MAP"},
	{CHK_SOLID_BGND, "SOLID_BGND"},
	{CHK_V_GRADIENT, "V_GRADIENT"},
	{CHK_USE_BIT_MAP, "USE_BIT_MAP"},
	{CHK_USE_SOLID_BGND, "USE_SOLID_BGND"},
	{CHK_USE_V_GRADIENT, "USE_V_GRADIENT"},
	{CHK_FOG, "FOG"},
	{CHK_FOG_BGND, "FOG_BGND"},
	{CHK_LAYER_FOG, "LAYER_FOG"},
	{CHK_DISTANCE_CUE, "DISTANCE_CUE"},
	{CHK_DCUE_BGND, "DCUE_BGND"},
	{CHK_USE_FOG, "USE_FOG"},
	{CHK_USE_LAYER_FOG, "USE_LAYER_FOG"},
	{CHK_USE_DISTANCE_CUE, "USE_DISTANCE_CUE"},
	{CHK_MAT_ENTRY, "MAT_ENTRY"},
	{CHK_MAT_NAME, "MAT_NAME"},
	{CHK_MAT_AMBIENT, "MAT_AMBIENT"},
	{CHK_MAT_DIFFUSE, "MAT_DIFFUSE"},
	{CHK_MAT_SPECULAR, "MAT_SPECULAR"},
	{CHK_MAT_SHININESS, "MAT_SHININESS"},
	{CHK_MAT_SHIN2PCT, "MAT_SHIN2PCT"},
	{CHK_MAT_TRANSPARENCY, "MAT_TRANSPARENCY"},
	{CHK_MAT_XPFALL, "MAT_XPFALL"},
	{CHK_MAT_USE_XPFALL, "MAT_USE_XPFALL"},
	{CHK_MAT_REFBLUR, "MAT_REFBLUR"},
	{CHK_MAT_SHADING, "MAT_SHADING"},
	{CHK_MAT_USE_REFBLUR, "MAT_USE_REFBLUR"},
	{CHK_MAT_SELF_ILLUM, "MAT_SELF_ILLUM"},
	{CHK_MAT_TWO_SIDE, "MAT_TWO_SIDE"},
	{CHK_MAT_DECAL, "MAT_DECAL"},
	{CHK_MAT_ADDITIVE, "MAT_ADDITIVE"},
	{CHK_MAT_SELF_ILPCT, "MAT_SELF_ILPCT"},
	{CHK_MAT_WIRE, "MAT_WIRE"},
	{CHK_MAT_FACEMAP, "MAT_FACEMAP"},
	{CHK_MAT_PHONGSOFT, "MAT_PHONGSOFT"},
	{CHK_MAT_WIREABS, "MAT_WIREABS"},
	{CHK_MAT_WIRE_SIZE, "MAT_WIRE_SIZE"},
	{CHK_MAT_TEXMAP, "MAT_TEXMAP"},
	{CHK_MAT_SXP_TEXT_DATA, "MAT_SXP_TEXT_DATA"},
	{CHK_MAT_TEXMASK, "MAT_TEXMASK"},
	{CHK_MAT_SXP_TEXTMASK_DATA, "MAT_SXP_TEXTMASK_DATA"},
	{CHK_MAT_TEX2MAP, "MAT_TEX2MAP"},
	{CHK_MAT_SXP_TEXT2_DATA, "MAT_SXP_TEXT2_DATA"},
	{CHK_MAT_TEX2MASK, "MAT_TEX2MASK"},
	{CHK_MAT_SXP_TEXT2MASK_DATA, "MAT_SXP_TEXT2MASK_DATA"},
	{CHK_MAT_OPACMAP, "MAT_OPACMAP"},
	{CHK_MAT_SXP_OPAC_DATA, "MAT_SXP_OPAC_DATA"},
	{CHK_MAT_OPACMASK, "MAT_OPACMASK"},
	{CHK_MAT_SXP_OPACMASK_DATA, "MAT_SXP_OPACMASK_DATA"},
	{CHK_MAT_BUMPMAP, "MAT_BUMPMAP"},
	{CHK_MAT_SXP_BUMP_DATA, "MAT_SXP_BUMP_DATA"},
	{CHK_MAT_BUMPMASK, "MAT_BUMPMASK"},
	{CHK_MAT_SXP_BUMPMASK_DATA, "MAT_SXP_BUMPMASK_DATA"},
	{CHK_MAT_SPECMAP, "MAT_SPECMAP"},
	{CHK_MAT_SXP_SPEC_DATA, "MAT_SXP_SPEC_DATA"},
	{CHK_MAT_SPECMASK, "MAT_SPECMASK"},
	{CHK_MAT_SXP_SPECMASK_DATA, "MAT_SXP_SPECMASK_DATA"},
	{CHK_MAT_SHINMAP, "MAT_SHINMAP"},
	{CHK_MAT_SXP_SHIN_DATA, "MAT_SXP_SHIN_DATA"},
	{CHK_MAT_SHINMASK, "MAT_SHINMASK"},
	{CHK_MAT_SXP_SHINMASK_DATA, "MAT_SXP_SHINMASK_DATA"},
	{CHK_MAT_SELFIMAP, "MAT_SELFIMAP"},
	{CHK_MAT_SXP_SELFI_DATA, "MAT_SXP_SELFI_DATA"},
	{CHK_MAT_SELFIMASK, "MAT_SELFIMASK"},
	{CHK_MAT_SXP_SELFIMASK_DATA, "MAT_SXP_SELFIMASK_DATA"},
	{CHK_MAT_REFLMAP, "MAT_REFLMAP"},
	{CHK_MAT_REFLMASK, "MAT_REFLMASK"},
	{CHK_MAT_SXP_REFLMASK_DATA, "MAT_SXP_REFLMASK_DATA"},
	{CHK_MAT_ACUBIC, "MAT_ACUBIC"},
	{CHK_MAT_MAPNAME, "MAT_MAPNAME"},
	{CHK_MAT_MAP_TILING, "MAT_MAP_TILING"},
	{CHK_MAT_MAP_TEXBLUR, "MAT_MAP_TEXBLUR"},
	{CHK_MAT_MAP_USCALE, "MAT_MAP_USCALE"},
	{CHK_MAT_MAP_VSCALE, "MAT_MAP_VSCALE"},
	{CHK_MAT_MAP_UOFFSET, "MAT_MAP_UOFFSET"},
	{CHK_MAT_MAP_VOFFSET, "MAT_MAP_VOFFSET"},
	{CHK_MAT_MAP_ANG, "MAT_MAP_ANG"},
	{CHK_MAT_MAP_COL1, "MAT_MAP_COL1"},
	{CHK_MAT_MAP_COL2, "MAT_MAP_COL2"},
	{CHK_MAT_MAP_RCOL, "MAT_MAP_RCOL"},
	{CHK_MAT_MAP_GCOL, "MAT_MAP_GCOL"},
	{CHK_MAT_MAP_BCOL, "MAT_MAP_BCOL"},
	{CHK_NAMED_OBJECT, "NAMED_OBJECT"},
	{CHK_N_DIRECT_LIGHT, "N_DIRECT_LIGHT"},
	{CHK_DL_OFF, "DL_OFF"},
	{CHK_DL_OUTER_RANGE, "DL_OUTER_RANGE"},
	{CHK_DL_INNER_RANGE, "DL_INNER_RANGE"},
	{CHK_DL_MULTIPLIER, "DL_MULTIPLIER"},
	{CHK_DL_EXCLUDE, "DL_EXCLUDE"},
	{CHK_DL_ATTENUATE, "DL_ATTENUATE"},
	{CHK_DL_SPOTLIGHT, "DL_SPOTLIGHT"},
	{CHK_DL_SPOT_ROLL, "DL_SPOT_ROLL"},
	{CHK_DL_SHADOWED, "DL_SHADOWED"},
	{CHK_DL_LOCAL_SHADOW2, "DL_LOCAL_SHADOW2"},
	{CHK_DL_SEE_CONE, "DL_SEE_CONE"},
	{CHK_DL_SPOT_RECTANGULAR, "DL_SPOT_RECTANGULAR"},
	{CHK_DL_SPOT_ASPECT, "DL_SPOT_ASPECT"},
	{CHK_DL_SPOT_PROJECTOR, "DL_SPOT_PROJECTOR"},
	{CHK_DL_SPOT_OVERSHOOT, "DL_SPOT_OVERSHOOT"},
	{CHK_DL_RAY_BIAS, "DL_RAY_BIAS"},
	{CHK_DL_RAYSHAD, "DL_RAYSHAD"},
	{CHK_N_CAMERA, "N_CAMERA"},
	{CHK_CAM_SEE_CONE, "CAM_SEE_CONE"},
	{CHK_CAM_RANGES, "CAM_RANGES"},
	{CHK_OBJ_HIDDEN, "OBJ_HIDDEN"},
	{CHK_OBJ_VIS_LOFTER, "OBJ_VIS_LOFTER"},
	{CHK_OBJ_DOESNT_CAST, "OBJ_DOESNT_CAST"},
	{CHK_OBJ_DONT_RCVSHADOW, "OBJ_DONT_RCVSHADOW"},
	{CHK_OBJ_MATTE, "OBJ_MATTE"},
	{CHK_OBJ_FAST, "OBJ_FAST"},
	{CHK_OBJ_PROCEDURAL, "OBJ_PROCEDURAL"},
	{CHK_OBJ_FROZEN, "OBJ_FROZEN"},
	{CHK_N_TRI_OBJECT, "N_TRI_OBJECT"},
	{CHK_POINT_ARRAY, "POINT_ARRAY"},
	{CHK_POINT_FLAG_ARRAY, "POINT_FLAG_ARRAY"},
	{CHK_FACE_ARRAY, "FACE_ARRAY"},
	{CHK_MSH_MAT_GROUP, "MSH_MAT_GROUP"},
	{CHK_SMOOTH_GROUP, "SMOOTH_GROUP"},
	{CHK_MSH_BOXMAP, "MSH_BOXMAP"},
	{CHK_TEX_VERTS, "TEX_VERTS"},
	{CHK_MESH_MATRIX, "MESH_MATRIX"},
	{CHK_MESH_COLOR, "MESH_COLOR"},
	{CHK_MESH_TEXTURE_INFO, "MESH_TEXTURE_INFO"},
	{CHK_KFDATA, "KFDATA"},
	{CHK_KFHDR, "KFHDR"},
	{CHK_KFSEG, "KFSEG"},
	{CHK_KFCURTIME, "KFCURTIME"},
	{CHK_AMBIENT_NODE_TAG, "AMBIENT_NODE_TAG"},
	{CHK_OBJECT_NODE_TAG, "OBJECT_NODE_TAG"},
	{CHK_CAMERA_NODE_TAG, "CAMERA_NODE_TAG"},
	{CHK_TARGET_NODE_TAG, "TARGET_NODE_TAG"},
	{CHK_LIGHT_NODE_TAG, "LIGHT_NODE_TAG"},
	{CHK_L_TARGET_NODE_TAG, "L_TARGET_NODE_TAG"},
	{CHK_SPOTLIGHT_NODE_TAG, "SPOTLIGHT_NODE_TAG"},
	{CHK_NODE_ID, "NODE_ID"},
	{CHK_NODE_HDR, "NODE_HDR"},
	{CHK_PIVOT, "PIVOT"},
	{CHK_INSTANCE_NAME, "INSTANCE_NAME"},
	{CHK_MORPH_SMOOTH, "MORPH_SMOOTH"},
	{CHK_BOUNDBOX, "BOUNDBOX"},
	{CHK_POS_TRACK_TAG, "POS_TRACK_TAG"},
	{CHK_COL_TRACK_TAG, "COL_TRACK_TAG"},
	{CHK_ROT_TRACK_TAG, "ROT_TRACK_TAG"},
	{CHK_SCL_TRACK_TAG, "SCL_TRACK_TAG"},
	{CHK_MORPH_TRACK_TAG, "MORPH_TRACK_TAG"},
	{CHK_FOV_TRACK_TAG, "FOV_TRACK_TAG"},
	{CHK_ROLL_TRACK_TAG, "ROLL_TRACK_TAG"},
	{CHK_HOT_TRACK_TAG, "HOT_TRACK_TAG"},
	{CHK_FALL_TRACK_TAG, "FALL_TRACK_TAG"},
	{CHK_HIDE_TRACK_TAG, "HIDE_TRACK_TAG"},
	{CHK_POLY_2D, "POLY_2D"},
	{CHK_SHAPE_OK, "SHAPE_OK"},
	{CHK_SHAPE_NOT_OK, "SHAPE_NOT_OK"},
	{CHK_SHAPE_HOOK, "SHAPE_HOOK"},
	{CHK_PATH_3D, "PATH_3D"},
	{CHK_PATH_MATRIX, "PATH_MATRIX"},
	{CHK_SHAPE_2D, "SHAPE_2D"},
	{CHK_M_SCALE, "M_SCALE"},
	{CHK_M_TWIST, "M_TWIST"},
	{CHK_M_TEETER, "M_TEETER"},
	{CHK_M_FIT, "M_FIT"},
	{CHK_M_BEVEL, "M_BEVEL"},
	{CHK_XZ_CURVE, "XZ_CURVE"},
	{CHK_YZ_CURVE, "YZ_CURVE"},
	{CHK_INTERPCT, "INTERPCT"},
	{CHK_DEFORM_LIMIT, "DEFORM_LIMIT"},
	{CHK_USE_CONTOUR, "USE_CONTOUR"},
	{CHK_USE_TWEEN, "USE_TWEEN"},
	{CHK_USE_SCALE, "USE_SCALE"},
	{CHK_USE_TWIST, "USE_TWIST"},
	{CHK_USE_TEETER, "USE_TEETER"},
	{CHK_USE_FIT, "USE_FIT"},
	{CHK_USE_BEVEL, "USE_BEVEL"},
	{CHK_DEFAULT_VIEW, "DEFAULT_VIEW"},
	{CHK_VIEW_TOP, "VIEW_TOP"},
	{CHK_VIEW_BOTTOM, "VIEW_BOTTOM"},
	{CHK_VIEW_LEFT, "VIEW_LEFT"},
	{CHK_VIEW_RIGHT, "VIEW_RIGHT"},
	{CHK_VIEW_FRONT, "VIEW_FRONT"},
	{CHK_VIEW_BACK, "VIEW_BACK"},
	{CHK_VIEW_USER, "VIEW_USER"},
	{CHK_VIEW_CAMERA, "VIEW_CAMERA"},
	{CHK_VIEW_WINDOW, "VIEW_WINDOW"},
	{CHK_VIEWPORT_LAYOUT_OLD, "VIEWPORT_LAYOUT_OLD"},
	{CHK_VIEWPORT_DATA_OLD, "VIEWPORT_DATA_OLD"},
	{CHK_VIEWPORT_LAYOUT, "VIEWPORT_LAYOUT"},
	{CHK_VIEWPORT_DATA, "VIEWPORT_DATA"},
	{CHK_VIEWPORT_DATA_3, "VIEWPORT_DATA_3"},
	{CHK_VIEWPORT_SIZE, "VIEWPORT_SIZE"},
	{CHK_NETWORK_VIEW, "NETWORK_VIEW"},
	{0, 0}
};


const char *
lib3ds_chunk_name(uint16_t chunk) {
	Lib3dsChunkTable *p;

	for (p = lib3ds_chunk_table; p->name != 0; ++p) {
		if (p->chunk == chunk) {
			return (p->name);
		}
	}
	return ("***UNKNOWN***");
}
