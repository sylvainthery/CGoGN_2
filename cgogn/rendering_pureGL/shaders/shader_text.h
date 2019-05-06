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

#ifndef CGOGN_RENDERING_SHADERS_TEXT_H_
#define CGOGN_RENDERING_SHADERS_TEXT_H_


#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>
#include <cgogn/rendering_pureGL/texture.h>


namespace cgogn
{

namespace rendering_pgl
{

// forward
class ShaderParamText;

class CGOGN_RENDERING_PUREGL_EXPORT ShaderText : public ShaderProgram
{
public:
	using  Self  = ShaderText;
	using  Param = ShaderParamText;
	friend Param;

protected:
	ShaderText();
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(ShaderText);
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


class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamText : public ShaderParam
{
	inline void set_uniforms() override
	{
		shader_->set_uniforms_values(texture_->bind(0),italic_);
	}

public:
	Texture2D* texture_;
	float32 italic_;


	using LocalShader = ShaderText;

	ShaderParamText(LocalShader* sh) :
		ShaderParam(sh),
		italic_(0)
	{}

	inline ~ShaderParamText() override {}

	inline void set_vbos(VBO* vbo_pos, VBO* vbo_str, VBO* vbo_colsize)
	{
		bind_vao();
		vbo_pos->associate(ShaderProgram::ATTRIB_POS);
		vbo_str->associate(ShaderProgram::ATTRIB_CUSTOM1);
		vbo_colsize->associate(ShaderProgram::ATTRIB_CUSTOM2);
		release_vao();
	}

};


} // namespace rendering_pgl
} // namespace cgogn

#endif
