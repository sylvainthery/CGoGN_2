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

#ifndef CGOGN_RENDERING_SHADERS_SHADERPROGRAM_H_
#define CGOGN_RENDERING_SHADERS_SHADERPROGRAM_H_

#include <cgogn/core/utils/unique_ptr.h>
#include <cgogn/rendering_pureGL/types.h>
#include <cgogn/rendering_pureGL/vao.h>
#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>

#include <iostream>
#include <cassert>
#include <memory>


namespace cgogn
{

namespace rendering_pgl
{
//using GLMat4d = Eigen::Matrix4d;
//using GLMat3d = Eigen::Matrix3d;
//using GLMat4  = Eigen::Matrix4f;
//using GLMat3  = Eigen::Matrix3f;
//using GLColor = Eigen::Vector4f;
//using GLVec2  = Eigen::Vector2f;
//using GLVec3  = Eigen::Vector3f;
//using GLVec4  = Eigen::Vector4f;

inline GLColor Color(uint8 R, uint8 G, uint8 B, uint8 A=255u)
{ return GLColor(float32(R)/255.0f,float32(G)/255.0f,float32(B)/255.0f,float32(A)/255.0f);}

// convenient conversion function
inline void* void_ptr(uint32 x)
{
	return reinterpret_cast<void*>(uint64_t(x));
}


class CGOGN_RENDERING_PUREGL_EXPORT Shader
{
protected:
	GLuint id_;

public:
	Shader() = delete;
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	inline Shader(GLenum type)
	{
		 id_ = glCreateShader(type);
	}

	inline ~Shader()
	{
		glDeleteShader(id_);
	}

	inline GLuint shaderId() const
	{
		return id_;
	}

	inline void compile(const std::string& src);
};

class CGOGN_RENDERING_PUREGL_EXPORT ShaderProgram
{
protected:

	static std::vector<ShaderProgram*>* instances_;
	GLuint id_;
	Shader* vert_shader_;
	Shader* frag_shader_;
	Shader* geom_shader_;
	virtual void set_locations() = 0;
	void load(const std::string& vert_src, const std::string& frag_src);
	void load(const std::string& vert_src, const std::string& frag_src, const std::string& geom_src);

	GLint unif_mvp_matrix_;
	GLint unif_mv_matrix_;
	GLint unif_projection_matrix_;
	GLint unif_normal_matrix_;

	std::vector<GLint> uniforms_;
public:

	enum Attrib_Indices: GLuint
	{
		ATTRIB_POS     = 1u,
		ATTRIB_COLOR   = 2u,
		ATTRIB_NORM    = 3u,
		ATTRIB_TC      = 4u,
		ATTRIB_SIZE    = 5u,
		ATTRIB_CUSTOM1 = 5u,
		ATTRIB_CUSTOM2 = 6u,
		ATTRIB_CUSTOM3 = 7u,
		ATTRIB_CUSTOM4 = 8u,
	};

	static void register_instance(ShaderProgram* sh);

	static void clean_all();

	ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	virtual ~ShaderProgram();

	inline GLuint id() const		{ return id_; }

	inline void start_use()			{ glUseProgram(id_); }

	inline void stop_use()			{ glUseProgram(0); }

	inline void bind()				{ glUseProgram(id_); }

	inline void release()			{ glUseProgram(0); }

	inline GLint uniform_location(const GLchar* str) const
	{
		return glGetUniformLocation(id_,str);
	}

	inline void add_uniform(const GLchar* str)
	{
		uniforms_.push_back(glGetUniformLocation(id_,str));
	}

	template<typename T1>
	void add_uniforms(T1 p1)
	{
		add_uniform(p1);
	}

	template<typename T1, typename... Ts>
	void add_uniforms(T1 p1, Ts... pn)
	{
		add_uniform(p1);
		add_uniforms(pn...);
	}



	inline void bind_attrib_location(GLuint attrib, const char* str_var)
	{
		glBindAttribLocation(id_, attrib, str_var);
	}


	inline void set_uniform_value(std::size_t i, const float32 v) { glUniform1f(uniforms_[i],v);}
	inline void set_uniform_value(std::size_t i, const GLVec2& v) { glUniform2fv(uniforms_[i],1,v.data());}
	inline void set_uniform_value(std::size_t i, const GLVec3& v) { glUniform3fv(uniforms_[i],1,v.data());}
	inline void set_uniform_value(std::size_t i, const GLVec4& v) { glUniform4fv(uniforms_[i],1,v.data());}
	inline void set_uniform_value(std::size_t i, const int32 v)   { glUniform1i(uniforms_[i],v);}
	inline void set_uniform_value(std::size_t i, const uint32 v)  { glUniform1ui(uniforms_[i],v);}
	inline void set_uniform_value(std::size_t i, const bool v)    { glUniform1i(uniforms_[i],int32(v));}

	template<typename T1>
	void set_uniforms_values(T1 p1)
	{
		set_uniform_value(uniforms_.size()-1,p1);
	}

	template<typename T1, typename... Ts>
	void set_uniforms_values(T1 p1, Ts... pn)
	{
		set_uniform_value(uniforms_.size()-1-sizeof...(pn),p1);
		set_uniforms_values(pn...);
	}

	void get_matrices_uniforms();

	void set_matrices(const GLMat4& proj, const GLMat4& mv);
	void set_matrices(const Mat4d& proj, const Mat4d& mv);

	void set_view_matrix(const GLMat4& mv);
	void set_view_matrix(const Mat4d& mv);

};


class CGOGN_RENDERING_PUREGL_EXPORT ShaderParam
{
protected:

	ShaderProgram* shader_;
	std::unique_ptr<VAO> vao_;
	virtual void set_uniforms() = 0;

	static const GLColor color_front_default;
	static const GLColor color_back_default;
	static const GLColor color_ambiant_default;
	static const GLColor color_spec_default;
	static const GLColor color_line_default;
	static const GLColor color_point_default;

public:

	ShaderParam(ShaderProgram* prg);
	ShaderParam(const ShaderParam&) = delete;
	ShaderParam& operator=(const ShaderParam&) = delete;
	inline virtual ~ShaderParam() {}

	inline void bind_vao()
	{
		vao_->bind();
	}

	inline void release_vao()
	{
		vao_->release();
	}

	inline ShaderProgram* get_shader() { return shader_; }
	/**
	 * @brief bind the shader set uniforms & matrices, bind vao
	 * @param proj projectiob matrix
	 * @param mv model-view matrix
	 */
	void bind(const GLMat4& proj, const GLMat4& mv);

	void bind();

	/**
	 * @brief release vao and shader
	 */
	void release();
};

} // namespace rendering_pgl

} // namespace cgogn

#endif // CGOGN_RENDERING_SHADERS_SHADERPROGRAM_H_
