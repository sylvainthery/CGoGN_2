/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* Copyright (C) 2015, IGG Group, ICube, University of Strasbourg, France       *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef CGOGN_RENDERING_SHADERS_EXPLODE_VOLUMES_H_
#define CGOGN_RENDERING_SHADERS_EXPLODE_VOLUMES_H_


#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>

namespace cgogn
{

namespace rendering_pgl
{

DECLARE_SHADER_CLASS(ExplodeVolumes)


class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamExplodeVolumes : public ShaderParam
{
	inline void set_uniforms() override
	{
		shader_->set_uniforms_values(color_,light_pos_,explode_vol_,plane_clip_,plane_clip2_);
	}

public:
	GLColor color_;
	GLVec3 light_pos_;
	float32 explode_vol_;
	GLVec4 plane_clip_;
	GLVec4 plane_clip2_;

	using LocalShader = ShaderExplodeVolumes;

	ShaderParamExplodeVolumes(LocalShader* sh) :
		ShaderParam(sh),
		color_(color_front_default),
		light_pos_(10, 100, 1000),
		explode_vol_(0.8f),
		plane_clip_(0,0,0,0),
		plane_clip2_(0,0,0,0)
	{}

	inline ~ShaderParamExplodeVolumes() override {}

	inline void set_vbos(VBO* vbo_pos)
	{
		bind_vao();
		associate_vbos(vbo_pos);
		release_vao();
	}

};


} // namespace rendering_pgl
} // namespace cgogn

#endif
