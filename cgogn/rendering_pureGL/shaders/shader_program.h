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

	inline void compileShader(const std::string& src)
	{
		const char* csrc = src.c_str();
		glShaderSource(id_, 1, &csrc, nullptr);
		glCompileShader(id_);


		int infologLength = 0;
			int charsWritten  = 0;
			char *infoLog;

			glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &infologLength);

			if(infologLength > 1)
			{
				infoLog = (char *)malloc(infologLength+1);
				glGetShaderInfoLog(id_, infologLength, &charsWritten, infoLog);

				std::cerr << "----------------------------------------" << std::endl << "compilation de " << "msg" <<  " : "<<std::endl << infoLog <<std::endl<< "--------"<< std::endl;

				std::string errors(infoLog);
				std::istringstream sserr(errors);
				std::vector<int> error_lines;
				std::string line;
				std::getline(sserr, line);
				while (! sserr.eof())
				{
					std::size_t a =0;
					while ((a<line.size()) && (line[a]>='0') && (line[a]<='9')) a++;
					std::size_t b =a+1;
					while ((b<line.size()) && (line[b]>='0') && (line[b]<='9')) b++;
					if (b<line.size())
					{
						int ln = std::stoi(line.substr(a+1, b-a-1));
						error_lines.push_back(ln);
					}
					std::getline(sserr, line);
				}

				free(infoLog);

				char* source = new char[16*1024];
				GLsizei length;
				glGetShaderSource(id_,16*1024,&length,source);
				std::string src(source);
				std::istringstream sssrc(src);
				int l = 1;
				while (! sssrc.eof())
				{
					std::getline(sssrc, line);
					std::cerr.width(3);
					auto it = std::find(error_lines.begin(),error_lines.end(),l);
					if (it != error_lines.end())
						std::cerr << "\033[41m\033[37m" << "EEEEEE" << line <<"\033[m" << std::endl;
					else
						std::cerr<< l << " : " << line << std::endl;
					l++;
				}
				std::cerr << "----------------------------------------" << std::endl;
		}



	}

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

	inline void bind_attrib_location(GLuint attrib, const char* str_var)
	{
		glBindAttribLocation(id_, attrib, str_var);
	}


	inline static void set_uniform_value(GLint u, const float32 v) { glUniform1f(u,v);}
	inline static void set_uniform_value(GLint u, const GLVec2& v) { glUniform2fv(u,1,v.data());}
	inline static void set_uniform_value(GLint u, const GLVec3& v) { glUniform3fv(u,1,v.data());}
	inline static void set_uniform_value(GLint u, const GLVec4& v) { glUniform4fv(u,1,v.data());}
	inline static void set_uniform_value(GLint u, const int32 v)   { glUniform1i(u,v);}
	inline static void set_uniform_value(GLint u, const bool v)  { glUniform1i(u,int32(v));}

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

public:

	ShaderParam(ShaderProgram* prg);
	ShaderParam(const ShaderParam&) = delete;
	ShaderParam& operator=(const ShaderParam&) = delete;
	virtual ~ShaderParam();

//	inline ShaderProgram* get_shader()
//	{
//		return shader_;
//	}

	/**
	 * @brief bind vao (and set uniform)
	 * @param with_uniforms ask to set uniforms
	 */
	void bind_vao_only(bool with_uniforms = true);

	/**
	 * @brief release vao
	 */
	void release_vao_only();

	/**
	 * @brief bind the shader set uniforms & matrices, bind vao
	 * @param proj projectiob matrix
	 * @param mv model-view matrix
	 */
	void bind(const GLMat4& proj, const GLMat4& mv);

	/**
	 * @brief release vao and shader
	 */
	void release();
};

} // namespace rendering_pgl

} // namespace cgogn

#endif // CGOGN_RENDERING_SHADERS_SHADERPROGRAM_H_
