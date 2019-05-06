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

#ifndef CGOGN_RENDERING_SHADERS_FLAT_COLOR_H_
#define CGOGN_RENDERING_SHADERS_FLAT_COLOR_H_

#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>


namespace cgogn
{

namespace rendering_pgl
{

// forward
class ShaderParamFlatColor;

class CGOGN_RENDERING_PUREGL_EXPORT ShaderFlatColor : public ShaderProgram
{
public:
	using Self  = ShaderFlatColor;
	using Param = ShaderParamFlatColor;
	friend Param;

protected:
	ShaderFlatColor();
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(ShaderFlatColor);
	void set_locations();
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


class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamFlatColor : public ShaderParam
{
	inline void set_uniforms() override
	{
		shader_->set_uniforms_values(ambiant_color_,light_pos_,bf_culling_);
	}


public:
	GLColor ambiant_color_;
	GLVec3 light_pos_;
	bool bf_culling_;

	using LocalShader = ShaderFlatColor;

	ShaderParamFlatColor(LocalShader* sh) :
		ShaderParam(sh),
		ambiant_color_(0.05f, 0.05f, 0.5f, 1),
		light_pos_(10, 100, 1000),
		bf_culling_(false)
	{}

	inline ~ShaderParamFlatColor() override {}

	inline void set_vbos(VBO* vbo_pos, VBO* vbo_color)
	{
		bind_vao();
		vbo_pos->associate(ShaderProgram::ATTRIB_POS);
		vbo_color->associate(ShaderProgram::ATTRIB_COLOR);
		release_vao();
	}
};


} // namespace rendering_pgl

} // namespace cgogn

#endif // CGOGN_RENDERING_SHADERS_FLAT_H_
