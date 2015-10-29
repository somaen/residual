/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_NODE_H
#define INCLUDED_LIB3DS_NODE_H
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

Lib3dsNode *lib3ds_node_new(Lib3dsNodeType type);
Lib3dsAmbientColorNode *lib3ds_node_new_ambient_color(float color0[3]);
Lib3dsCameraNode *lib3ds_node_new_camera(Lib3dsCamera *camera);
Lib3dsTargetNode *lib3ds_node_new_camera_target(Lib3dsCamera *camera);
Lib3dsOmnilightNode *lib3ds_node_new_omnilight(Lib3dsLight *light);
Lib3dsSpotlightNode *lib3ds_node_new_spotlight(Lib3dsLight *light);
Lib3dsTargetNode *lib3ds_node_new_spotlight_target(Lib3dsLight *light);
void lib3ds_node_free(Lib3dsNode *node);
void lib3ds_node_eval(Lib3dsNode *node, float t);
Lib3dsNode *lib3ds_node_by_name(Lib3dsNode *node, const Common::String &name, Lib3dsNodeType type);
Lib3dsNode *lib3ds_node_by_id(Lib3dsNode *node, unsigned short node_id);
Lib3dsMeshInstanceNode *lib3ds_node_new_mesh_instance(Lib3dsMesh *mesh, const char *instance_name, float pos0[3], float scl0[3], float rot0[4]);

/** @} */
#endif
