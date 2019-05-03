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

#ifndef CGOGN_RENDERING_SHADERS_FLAT_H_
#define CGOGN_RENDERING_SHADERS_FLAT_H_

#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>

namespace cgogn
{

namespace rendering_pgl
{

// forward
class ShaderParamFlat;

class CGOGN_RENDERING_PUREGL_EXPORT ShaderFlat : public ShaderProgram
{
public:
	using Self  = ShaderFlat;
	using Param = ShaderParamFlat;
	friend Param;
protected:
	ShaderFlat();
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(ShaderFlat);
	void set_locations();
	static Self* instance_;

	/// uniform ids
	GLint unif_front_color_;
	GLint unif_back_color_;
	GLint unif_ambiant_color_;
	GLint unif_light_position_;
	GLint unif_bf_culling_;

public:

	static std::unique_ptr<Param> generate_param();
};


class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamFlat : public ShaderParam
{

public:
	using LocalShader = ShaderFlat;

	GLColor front_color_;
	GLColor back_color_;
	GLColor ambiant_color_;
	GLVec3 light_pos_;
	bool bf_culling_;

	ShaderParamFlat(LocalShader* sh) :
		ShaderParam(sh),
		ambiant_color_(0.25, 0.25, 0.25,1.0),
		light_pos_(10, 100, 1000),
		bf_culling_(false)
	{}


	inline virtual ~ShaderParamFlat() override
	{}

	inline void set_vbos(VBO* vbo_pos)
	{
		vao_->bind();
		vbo_pos->associate(ShaderProgram::ATTRIB_POS);
		vao_->release();
	}

protected:
	inline LocalShader* get_shader()
	{
		return static_cast<LocalShader*>(shader_);
	}

	inline void set_uniforms() override
	{
		ShaderProgram::set_uniform_value(get_shader()->unif_front_color_, front_color_);
		ShaderProgram::set_uniform_value(get_shader()->unif_back_color_, back_color_);
		ShaderProgram::set_uniform_value(get_shader()->unif_ambiant_color_, ambiant_color_);
		ShaderProgram::set_uniform_value(get_shader()->unif_light_position_, light_pos_);
		ShaderProgram::set_uniform_value(get_shader()->unif_bf_culling_, bf_culling_);
	}
};


} // namespace rendering_pgl

} // namespace cgogn

#endif // CGOGN_RENDERING_SHADERS_FLAT_H_
