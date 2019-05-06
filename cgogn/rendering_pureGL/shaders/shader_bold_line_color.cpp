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

#define CGOGN_RENDER_SHADERS_BOLD_LINE_CPP_

#include <iostream>

#include <cgogn/core/utils/logger.h>
#include <cgogn/rendering_pureGL/shaders/shader_bold_line_color.h>

namespace cgogn
{

namespace rendering_pgl
{

ShaderBoldLineColor* ShaderBoldLineColor::instance_ = nullptr;


static const char* vertex_shader_source =
"#version 150\n"
"in vec3 vertex_pos;\n"
"in vec3 vertex_color;\n"
"out vec3 color_v;\n"
"void main()\n"
"{\n"
"   color_v = vertex_color;\n"
"   gl_Position = vec4(vertex_pos,1.0);\n"
"}\n";

static const char* geometry_shader_source =
"#version 150\n"
"layout (lines) in;\n"
"layout (triangle_strip, max_vertices=6) out;\n"
"in vec3 color_v[];\n"
"out vec4 color_f;\n"
"out vec4 posi_clip;\n"
"uniform mat4 projection_matrix;\n"
"uniform mat4 model_view_matrix;\n"
"uniform vec2 lineWidths;\n"
"void main()\n"
"{\n"
"	vec4 A = model_view_matrix * gl_in[0].gl_Position;\n"
"	vec4 B = model_view_matrix * gl_in[1].gl_Position;\n"
"	float nearZ = 1.0;\n"
"	if (projection_matrix[2][2] !=  1.0)\n"
"		nearZ = - projection_matrix[3][2] / (projection_matrix[2][2] - 1.0); \n"
"	if ((A.z < nearZ) || (B.z < nearZ))\n"
"	{\n"
"		if (A.z >= nearZ)\n"
"			A = B + (A-B)*(nearZ-B.z)/(A.z-B.z);\n"
"		if (B.z >= nearZ)\n"
"			B = A + (B-A)*(nearZ-A.z)/(B.z-A.z);\n"
"		A = projection_matrix*A;\n"
"		B = projection_matrix*B;\n"
"		A = A/A.w;\n"
"		B = B/B.w;\n"
"		vec2 U2 = normalize(vec2(lineWidths[1],lineWidths[0])*(B.xy - A.xy));\n"
"		vec2 LWCorr =lineWidths * max(abs(U2.x),abs(U2.y));\n"
"		vec3 U = vec3(LWCorr*U2,0.0);\n"
"		vec3 V = vec3(LWCorr*vec2(U2[1], -U2[0]), 0.0);	\n"
"		color_f = vec4(color_v[0],0.0);\n"
"		posi_clip = gl_in[0].gl_Position;\n"
"		gl_Position = vec4(A.xyz-V, 1.0);\n"
"		EmitVertex();\n"
"		color_f = vec4(color_v[1],0.0);\n"
"		posi_clip = gl_in[1].gl_Position;\n"
"		gl_Position = vec4(B.xyz-V, 1.0);\n"
"		EmitVertex();\n"
"		color_f = vec4(color_v[0],1.0);\n"
"		posi_clip = gl_in[0].gl_Position;\n"
"		gl_Position = vec4(A.xyz-U, 1.0);\n"
"		EmitVertex();\n"
"		color_f = vec4(color_v[1],1.0);\n"
"		posi_clip = gl_in[1].gl_Position;\n"
"		gl_Position = vec4(B.xyz+U, 1.0);\n"
"		EmitVertex();\n"
"		color_f = vec4(color_v[0],0.0);\n"
"		posi_clip = gl_in[0].gl_Position;\n"
"		gl_Position = vec4(A.xyz+V, 1.0);\n"
"		EmitVertex();\n"
"		color_f = vec4(color_v[1],0.0);\n"
"		posi_clip = gl_in[1].gl_Position;\n"
"		gl_Position = vec4(B.xyz+V, 1.0);\n"
"		EmitVertex();\n"
"		EndPrimitive();\n"
"	}\n"
"}\n";


static const char* fragment_shader_source =
"#version 150\n"
"uniform vec4 plane_clip;\n"
"uniform vec4 plane_clip2;\n"
"in vec4 color_f;\n"
"in vec4 posi_clip;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"	float d = dot(plane_clip,posi_clip);\n"
"	float d2 = dot(plane_clip2,posi_clip);\n"
"	if ((d>0.0)||(d2>0.0))  discard;\n"
"   fragColor = color_f;\n"
"}\n";



void ShaderBoldLineColor::set_locations()
{
	bind_attrib_location(ATTRIB_POS, "vertex_pos");
	bind_attrib_location(ATTRIB_COLOR, "vertex_color");
}

ShaderBoldLineColor::ShaderBoldLineColor()
{
	load(vertex_shader_source,fragment_shader_source, geometry_shader_source);
	add_uniforms("lineWidths","plane_clip","plane_clip2");
}


}
} // namespace cgogn
