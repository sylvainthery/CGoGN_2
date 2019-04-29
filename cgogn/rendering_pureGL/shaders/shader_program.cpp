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


#include <cgogn/core/utils/unique_ptr.h>
#include <cgogn/rendering/shaders/shader_program.h>

namespace cgogn
{

namespace rendering
{

ShaderProgram::ShaderProgram():
	vert_shader_(nullptr),
	frag_shader_(nullptr),
	geom_shader_(nullptr)
{
	id_ = glCreateProgram();
}

//ShaderProgram::ShaderProgram(const std::string& vert_src, const std::string& frag_src):
//	vert_shader_(nullptr),
//	frag_shader_(nullptr),
//	geom_shader_(nullptr)
//{
//	id_ = glCreateProgram();

//	load(vert_src, frag_src);
//}

//ShaderProgram::ShaderProgram(const std::string& vert_src, const std::string& frag_src, const std::string& geom_src):
//	vert_shader_(nullptr),
//	frag_shader_(nullptr),
//	geom_shader_(nullptr)
//{
//	id_ = glCreateProgram();

//	load(vert_src, frag_src, geom_src);
//}


ShaderProgram::~ShaderProgram()
{
	delete vert_shader_;
	delete frag_shader_;
	delete frag_shader_;
	glDeleteProgram(id_);
}

void ShaderProgram::load(const std::string& vert_src, const std::string& frag_src)
{
	vert_shader_ = new Shader(GL_VERTEX_SHADER);
	vert_shader_->compileShader(vert_src);

	frag_shader_ = new Shader(GL_FRAGMENT_SHADER);
	frag_shader_->compileShader(frag_src);

	glAttachShader(id_, vert_shader_->shaderId());
	glAttachShader(id_, frag_shader_->shaderId());

	set_locations();

	glLinkProgram(id_);

	// puis detache (?)
	glDetachShader(id_, frag_shader_->shaderId());
	glDetachShader(id_, vert_shader_->shaderId());
}



std::vector<ShaderProgram*>* ShaderProgram::instances_ = nullptr;

void ShaderProgram::register_instance(ShaderProgram* sh)
{
	if (instances_ == nullptr)
	{
		instances_ = new std::vector<ShaderProgram*>;
		instances_->reserve(256);
	}

	auto it = std::find(instances_->begin(), instances_->end(), sh);
	if (it == instances_->end())
		instances_->push_back(sh);
}

void ShaderProgram::clean_all()
{
	if (instances_ != nullptr)
	{
		for (auto* ptr : *instances_)
			delete ptr;
		delete instances_;
		instances_ = nullptr;
	}
}


void ShaderProgram::get_matrices_uniforms()
{
	unif_mvp_matrix_ = glGetUniformLocation(id_,"mvp_matrix");
	unif_mv_matrix_ = glGetUniformLocation(id_,"model_view_matrix");
	unif_projection_matrix_ = glGetUniformLocation(id_,"projection_matrix");
	unif_normal_matrix_ = glGetUniformLocation(id_,"normal_matrix");
}

void ShaderProgram::set_matrices(const GLMat4d& proj, const GLMat4d& mv)
{
	if (unif_mvp_matrix_ >= 0)
	{
		GLMat4d mvp = (proj*mv);
		GLMat4 m = mvp.cast<float>();
		glUniformMatrix4fv(	unif_mvp_matrix_,1,false, m.data());
	}
	if (unif_projection_matrix_ >= 0)
	{
		GLMat4 m = proj.cast<float>();
		glUniformMatrix4fv(	unif_projection_matrix_,1,false, m.data());
	}
	if (unif_mv_matrix_ >= 0)
	{
		GLMat4 m = mv.cast<float>();
		glUniformMatrix4fv(	unif_mv_matrix_,1,false, m.data());
	}

	if (unif_normal_matrix_ >= 0)
	{
		Eigen::Affine3d t(mv);
		GLMat3 normal_matrix = t.linear().inverse().transpose().matrix().cast<float>();
		glUniformMatrix3fv(	unif_normal_matrix_,1,false, normal_matrix.data());
	}
}

void ShaderProgram::set_matrices(const GLMat4& proj, const GLMat4& mv)
{
	if (unif_mvp_matrix_ >= 0)
	{
		GLMat4 m = proj*mv;
		glUniformMatrix4fv(	unif_mvp_matrix_,1,false, m.data());
	}
	if (unif_projection_matrix_ >= 0)
		glUniformMatrix4fv(	unif_projection_matrix_,1,false, proj.data());
	if (unif_mv_matrix_ >= 0)
		glUniformMatrix4fv(	unif_mv_matrix_,1,false, mv.data());

	if (unif_normal_matrix_ >= 0)
	{
		Eigen::Affine3d t(mv);
		GLMat3 normal_matrix = t.linear().inverse().transpose().matrix().cast<float>();
		glUniformMatrix3fv(	unif_normal_matrix_,1,false, normal_matrix.data());
	}
}


void ShaderProgram::set_view_matrix(const GLMat4d& mv)
{
	if (unif_mv_matrix_ >= 0)
	{
		GLMat4 m = mv.cast<float>();
		glUniformMatrix4fv(	unif_mv_matrix_,1,false, m.data());
	}

	if (unif_normal_matrix_ >= 0)
	{
		Eigen::Affine3d t(mv);
		GLMat3 normal_matrix = t.linear().inverse().transpose().matrix().cast<float>();
		glUniformMatrix3fv(	unif_normal_matrix_,1,false, normal_matrix.data());
	}
}

void ShaderProgram::set_view_matrix(const GLMat4& mv)
{
	if (unif_mv_matrix_ >= 0)
		glUniformMatrix4fv(	unif_mv_matrix_,1,false, mv.data());

	if (unif_normal_matrix_ >= 0)
	{
		Eigen::Affine3d t(mv);
		GLMat3 normal_matrix = t.linear().inverse().transpose().matrix().cast<float>();
		glUniformMatrix3fv(	unif_normal_matrix_,1,false, normal_matrix.data());
	}
}



ShaderParam::ShaderParam(ShaderProgram* prg) :
	shader_(prg), vao_(nullptr)
{}

ShaderParam::~ShaderParam()
{
	delete vao_;
}

void ShaderParam::bind_vao_only(bool with_uniforms)
{
	shader_->bind();
	if (with_uniforms)
		set_uniforms();
	vao_->bind();
}

void ShaderParam::release_vao_only()
{
	vao_->release();
}

void ShaderParam::bind(const GLMat4& proj, const GLMat4& mv)
{
	shader_->bind();
	shader_->set_matrices(proj,mv);
	set_uniforms();
	vao_->bind();
}

void ShaderParam::release()
{
	vao_->release();
	shader_->release();
}


} // namespace rendering

} // namespace cgogn
