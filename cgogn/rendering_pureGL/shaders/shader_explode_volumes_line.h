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

#ifndef CGOGN_RENDERING_SHADERS_EXPLODE_VOLUMES_LINE_H_
#define CGOGN_RENDERING_SHADERS_EXPLODE_VOLUMES_LINE_H_


#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>

namespace cgogn
{

namespace rendering_pgl
{

// forward
class ShaderParamExplodeVolumesLine;

class CGOGN_RENDERING_PUREGL_EXPORT ShaderExplodeVolumesLine : public ShaderProgram
{
public:
	using  Self  = ShaderExplodeVolumesLine;
	using  Param = ShaderParamExplodeVolumesLine;
	friend Param;

protected:
	ShaderExplodeVolumesLine();
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(ShaderExplodeVolumesLine);
	void set_locations() override;
	static Self* instance_;

public:
	inline static std::unique_ptr<Param> generate_param()
	{
		if (!instance_)
		{
			instance_ = new Self();
			ShaderProgram::register_instance(instance_);
		}
		return cgogn::make_unique<Param>(instance_);
	}

};


class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamExplodeVolumesLine : public ShaderParam
{
	inline void set_uniforms() override
	{
		shader_->set_uniforms_values(color_,explode_factor_,plane_clip_,plane_clip2_);
	}

public:
	GLColor color_;
	float32 explode_factor_;
	GLVec4 plane_clip_;
	GLVec4 plane_clip2_;

	using LocalShader = ShaderExplodeVolumesLine;

	ShaderParamExplodeVolumesLine(LocalShader* sh) :
		ShaderParam(sh),
		color_(color_front_default),
		explode_factor_(0.8f),
		plane_clip_(0,0,0,0),
		plane_clip2_(0,0,0,0)
	{}

	inline ~ShaderParamExplodeVolumesLine() override {}

	inline void set_vbos(VBO* vbo_pos)
	{
		bind_vao();
		vbo_pos->associate(ShaderProgram::ATTRIB_POS);
		release_vao();
	}

};


} // namespace rendering_pgl
} // namespace cgogn

#endif
