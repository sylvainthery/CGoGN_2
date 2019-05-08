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

#define CGOGN_RENDER_SHADERS_PHONG_CPP_

#include <iostream>

#include <cgogn/rendering_pureGL/shaders/shader_phong_color.h>

namespace cgogn
{

namespace rendering_pgl
{

ShaderPhongColor* ShaderPhongColor::instance_ = nullptr;

static const char* vertex_shader_source =
	"#version 150\n"
	"in vec3 vertex_pos;\n"
	"in vec3 vertex_normal;\n"
	"in vec3 vertex_color;\n"
	"uniform mat4 projection_matrix;\n"
	"uniform mat4 model_view_matrix;\n"
	"uniform mat3 normal_matrix;\n"
	"uniform vec3 lightPosition;\n"
	"out vec3 EyeVector;\n"
	"out vec3 Normal;\n"
	"out vec3 LightDir;\n"
	"out vec3 front_color;\n"
	"void main ()\n"
	"{\n"
	"	Normal = normal_matrix * vertex_normal;\n"
	"	vec3 Position = vec3 (model_view_matrix * vec4 (vertex_pos, 1.0));\n"
	"	LightDir = lightPosition - Position;\n"
	"	EyeVector = -Position;"
	"	front_color = vertex_color;"
	"	gl_Position = projection_matrix * model_view_matrix * vec4 (vertex_pos, 1.0);\n"
	"}\n";

static const char* fragment_shader_source =
	"#version 150\n"
	"in vec3 EyeVector;\n"
	"in vec3 Normal;\n"
	"in vec3 LightDir;\n"
	"in vec3 front_color;\n"
	"uniform vec4 spec_color;\n"
	"uniform vec4 ambiant_color;\n"
	"uniform float spec_coef;\n"
	"uniform bool double_side;\n"
	"out vec4 frag_color;\n"
	"void main()\n"
	"{\n"
	"	vec3 N = normalize (Normal);\n"
	"	vec3 L = normalize (LightDir);\n"
	"	vec4 finalColor = ambiant_color;\n"
	"	if (gl_FrontFacing==false)\n" // do not use ! because of bug on old intel under OS/X
	"	{\n"
	"		if (!double_side)\n"
	"			discard;\n"
	"		N *= -1.0;\n"
	"	}\n"
	"	float lambertTerm = clamp(dot(N,L),0.0,1.0);\n"
	"	finalColor += vec4(front_color*lambertTerm,0.0);\n"
	"	vec3 E = normalize(EyeVector);\n"
	"	vec3 R = reflect(-L, N);\n"
	"	float specular = pow( max(dot(R, E), 0.0), spec_coef );\n"
	"	finalColor += spec_color * specular;\n"
	"	frag_color=finalColor;\n"
	"}\n";

ShaderPhongColor::ShaderPhongColor()
{

	load3_bind(vertex_shader_source,fragment_shader_source,
			  "vertex_pos", "vertex_normal", "vertex_color");

	add_uniforms("light_position",
				 "ambiant_color",
				 "spec_color",
				 "spec_coef",
				 "double_side");

}

} // namespace rendering_pgl
} // namespace cgogn
