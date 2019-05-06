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

#ifndef CGOGN_RENDERING_SHADERS_PHONG_COLOR_H_
#define CGOGN_RENDERING_SHADERS_PHONG_COLOR_H_

#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>

namespace cgogn
{

namespace rendering_pgl
{

class ShaderParamPhongColor;

class CGOGN_RENDERING_PUREGL_EXPORT ShaderPhongColor : public ShaderProgram
{
public:
	using  Self  = ShaderPhongColor;
	using  Param = ShaderParamPhongColor;
	friend Param;

	inline static std::unique_ptr<Param> generate_param()
	{
		if (!instance_)
		{
			instance_ = new Self();
			ShaderProgram::register_instance(instance_);
		}
		return cgogn::make_unique<Param>(instance_);
	}

protected:
	ShaderPhongColor();
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(ShaderPhongColor);
	void set_locations() override;
	static Self* instance_;
};

class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamPhongColor : public ShaderParam
{
protected:

	inline void set_uniforms() override
	{
		shader_->set_uniforms_values(light_position_,
					ambiant_color_,
					specular_color_,
					specular_coef_,
					double_side_);
	}

public:

	GLVec3 light_position_;
	GLColor ambiant_color_;
	GLColor specular_color_;
	float32 specular_coef_;
	bool double_side_;

	using ShaderType = ShaderPhongColor;

	ShaderParamPhongColor(ShaderType* sh) :
		ShaderParam(sh),
		light_position_(),
		ambiant_color_(),
		specular_color_(),
		specular_coef_(),
		double_side_()
	{}

	inline void set_vbos(VBO* vbo_pos, VBO* vbo_norm, VBO* vbo_color)
	{
		bind_vao();
		vbo_pos->associate(ShaderProgram::ATTRIB_POS);
		vbo_norm->associate(ShaderProgram::ATTRIB_NORM);
		vbo_norm->associate(ShaderProgram::ATTRIB_COLOR);
		release_vao();
	}
};
}
}
#endif // CGOGN_RENDERING_SHADERS_PHONG_H_
