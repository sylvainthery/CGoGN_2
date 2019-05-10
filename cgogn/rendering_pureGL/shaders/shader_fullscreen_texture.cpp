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


#include <cgogn/rendering_pureGL/shaders/shader_fullscreen_texture.h>

namespace cgogn
{

namespace rendering_pgl
{


ShaderFSTexture* ShaderFSTexture::instance_ = nullptr;

ShaderFSTexture::ShaderFSTexture()
{
	const char* vertex_shader_source =
			"#version 150\n"
			"out vec2 tc;\n"
			"void main()\n"
			"{\n"
			"	vec2 p = 2.0*vec2(gl_VertexID%2, gl_VertexID/2);\n"
			"	tc = p;\n"
			"	p = 2.0*p - 1.0; "
			"   gl_Position = vec4(p,0.0,1.0);\n"
			"}\n";
	const char* fragment_shader_source =
			"#version 150\n"
			"out vec4 frag_color;\n"
			"uniform sampler2D texture_unit;\n"
			"uniform float alpha;\n"
			"in vec2 tc;\n"
			"void main()\n"
			"{\n"
			"	frag_color = vec4(texture(texture_unit,tc).rgb,alpha);\n"
			"}\n";

	load(vertex_shader_source,fragment_shader_source);
	add_uniforms("texture_unit","alpha");
}

}
} // namespace cgogn
