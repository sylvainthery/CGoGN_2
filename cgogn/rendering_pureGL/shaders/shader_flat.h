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
#include <cgogn/rendering_pureGL/vbo.h>

namespace cgogn
{

namespace rendering_pgl
{

// forward
template <bool CPV>
class ShaderParamFlat: public ShaderParam
{};

class CGOGN_RENDERING_PUREGL_EXPORT ShaderFlatGen : public ShaderProgram
{
	template <bool CPV> friend class ShaderParamFlat;

protected:

	bool cpv_;

	static const char* vertex_shader_source_;
	static const char* fragment_shader_source_;

	static const char* vertex_shader_source2_;
	static const char* fragment_shader_source2_;

	// uniform ids
	GLint unif_front_color_;
	GLint unif_back_color_;
	GLint unif_ambiant_color_;
	GLint unif_light_position_;
	GLint unif_bf_culling_;

	void set_locations();
public:

	using Self = ShaderFlatGen;
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(ShaderFlatGen);

	enum Attrib_Indices: GLuint
	{
		ATTRIB_POS = 1u,
		ATTRIB_COLOR = 2u
	};

	/**
	 * @brief set current front color
	 * @param rgba
	 */
	void set_front_color(const GLColor& rgba);

	/**
	 * @brief set current front color
	 * @param rgba
	 */
	void set_back_color(const GLColor& rgba);

	/**
	 * @brief set current ambiant color
	 * @param rgba
	 */
	void set_ambiant_color(const GLColor& rgba);

	/**
	 * @brief set light position relative to screen
	 * @param l light position
	 */
	void set_light_position(const GLVec3& l);

	/**
	 * @brief set light position relative to world
	 * @param l light position
	 * @param view_matrix
	 */
	void set_local_light_position(const GLVec3& l, const GLMat4& view_matrix);

	void set_bf_culling(bool cull);

protected:

	ShaderFlatGen(bool color_per_vertex);
};

template <bool CPV>
class ShaderFlatTpl : public ShaderFlatGen
{
public:

	using Param = ShaderParamFlat<CPV>;
	inline static std::unique_ptr<Param> generate_param();

private:

	inline ShaderFlatTpl() : ShaderFlatGen(CPV) {}
};


// COLOR UNIFORM PARAM
template <>
class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamFlat<false> : public ShaderParam
{
protected:

	inline void set_uniforms() override
	{
		ShaderFlatGen* sh = static_cast<ShaderFlatGen*>(this->shader_);
		sh->set_front_color(front_color_);
		sh->set_back_color(back_color_);
		sh->set_ambiant_color(ambiant_color_);
		sh->set_light_position(light_pos_);
		sh->set_bf_culling(bf_culling_);
	}

public:

	GLColor front_color_;
	GLColor back_color_;
	GLColor ambiant_color_;
	GLVec3 light_pos_;
	bool bf_culling_;

	inline ShaderParamFlat(ShaderFlatGen* sh) :
		ShaderParam(sh),
		front_color_(0.95, 0.0, 0.0, 1.0),
		back_color_(0.0, 0.95, 0.0,1.0),
		ambiant_color_(0.25, 0.25, 0.25,1.0),
		light_pos_(10.0, 100.0, 1000.0),
		bf_culling_(false)
	{}

	virtual ~ShaderParamFlat() override;

	inline void set_position_vbo(VBO* vbo_pos)
	{
		vao_->bind();
		vbo_pos->bind();
		glEnableVertexAttribArray(ShaderFlatGen::ATTRIB_POS);
		glVertexAttribPointer(ShaderFlatGen::ATTRIB_POS, vbo_pos->vector_dimension(), GL_FLOAT, GL_FALSE, 0, nullptr);
		vbo_pos->release();
		vao_->release();
	}
};


// COLOR PER VERTEX PARAM
template <>
class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamFlat<true> : public ShaderParam
{
protected:

	inline void set_uniforms() override
	{
		ShaderFlatGen* sh = static_cast<ShaderFlatGen*>(this->shader_);
		sh->set_ambiant_color(ambiant_color_);
		sh->set_light_position(light_pos_);
		sh->set_bf_culling(bf_culling_);
	}

public:


	GLColor ambiant_color_;
	GLVec3 light_pos_;
	bool bf_culling_;

	inline ShaderParamFlat(ShaderFlatGen* sh) :
		ShaderParam(sh),
		ambiant_color_(0.25, 0.25, 0.25,1.0),
		light_pos_(10, 100, 1000),
		bf_culling_(false)
	{
		vao_->create();
	}

	virtual ~ShaderParamFlat() override;

	inline void set_all_vbos(VBO* vbo_pos, VBO* vbo_color)
	{
		shader_->bind();
		vao_->bind();
		// position
		vbo_pos->bind();
		glEnableVertexAttribArray(ShaderFlatGen::ATTRIB_POS);
		glVertexAttribPointer(ShaderFlatGen::ATTRIB_POS, vbo_pos->vector_dimension(), GL_FLOAT, GL_FALSE, 0, nullptr);
		vbo_pos->release();
		// color
		vbo_color->bind();
		glEnableVertexAttribArray(ShaderFlatGen::ATTRIB_COLOR);
		glVertexAttribPointer(ShaderFlatGen::ATTRIB_COLOR, vbo_color->vector_dimension(), GL_FLOAT, GL_FALSE, 0, nullptr);
		vbo_color->release();
		vao_->release();
		shader_->release();
	}

	inline void set_position_vbo(VBO* vbo_pos)
	{
		shader_->bind();
		vao_->bind();
		vbo_pos->bind();
		glEnableVertexAttribArray(ShaderFlatGen::ATTRIB_POS);
		glVertexAttribPointer(ShaderFlatGen::ATTRIB_POS, vbo_pos->vector_dimension(), GL_FLOAT, GL_FALSE, 0, nullptr);
		vbo_pos->release();
		vao_->release();
		shader_->release();
	}

	inline void set_color_vbo(VBO* vbo_color)
	{
		shader_->bind();
		vao_->bind();
		vbo_color->bind();
		glEnableVertexAttribArray(ShaderFlatGen::ATTRIB_COLOR);
		glVertexAttribPointer(ShaderFlatGen::ATTRIB_COLOR, vbo_color->vector_dimension(), GL_FLOAT, GL_FALSE, 0, nullptr);
		vbo_color->release();
		vao_->release();
		shader_->release();
	}
};

template <bool CPV>
std::unique_ptr<typename ShaderFlatTpl<CPV>::Param> ShaderFlatTpl<CPV>::generate_param()
{
	static ShaderFlatTpl* instance_;
	if (!instance_)
	{
		instance_ = new ShaderFlatTpl<CPV>();
		ShaderProgram::register_instance(instance_);
	}
	return cgogn::make_unique<Param>(instance_);
}

using ShaderFlat = ShaderFlatTpl<false>;
using ShaderFlatColor = ShaderFlatTpl<true>;


#if defined(CGOGN_USE_EXTERNAL_TEMPLATES) && !defined(CGOGN_RENDER_SHADERS_FLAT_CPP_)
extern template class CGOGN_RENDERING_EXPORT ShaderFlatTpl<false>;
extern template class CGOGN_RENDERING_EXPORT ShaderFlatTpl<true>;
#endif




} // namespace rendering_pgl

} // namespace cgogn

#endif // CGOGN_RENDERING_SHADERS_FLAT_H_
