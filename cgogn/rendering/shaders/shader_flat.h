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

#ifndef RENDERING_SHADERS_FLAT_H_
#define RENDERING_SHADERS_FLAT_H_

#include <rendering/shaders/shader_program.h>
#include <rendering/shaders/vbo.h>
#include <rendering/dll.h>

class QColor;

namespace cgogn
{

namespace rendering
{

class CGOGN_RENDERING_API ShaderFlat : public ShaderProgram
{
	static const char* vertex_shader_source_;
	static const char* fragment_shader_source_;

	static const char* vertex_shader_source2_;
	static const char* fragment_shader_source2_;

	enum
	{
		ATTRIB_POS = 0,
		ATTRIB_COLOR
	};

	// uniform ids
	int unif_front_color_;
	int unif_back_color_;
	int unif_ambiant_color_;
	int unif_light_position_;

public:

	ShaderFlat(bool color_per_vertex = false);

	/**
	 * @brief set current front color
	 * @param rgb
	 */
	void set_front_color(const QColor& rgb);

	/**
	 * @brief set current front color
	 * @param rgb
	 */
	void set_back_color(const QColor& rgb);

	/**
	 * @brief set current ambiant color
	 * @param rgb
	 */
	void set_ambiant_color(const QColor& rgb);

	/**
	 * @brief set light position relative to screen
	 * @param l light position
	 */
	void set_light_position(const QVector3D& l);


	/**
	 * @brief set light position relative to world
	 * @param l light position
	 * @param view_matrix
	 */
	void set_local_light_position(const QVector3D& l, const QMatrix4x4& view_matrix);

	/**
	 * @brief set a vao configuration
	 * @param i id of vao (0,1,....)
	 * @param vbo_pos pointer on position vbo (XYZ)
	 * @param vbo_color pointer on color vbo (RGB)
	 * @return true if ok
	 */
	bool set_vao(unsigned int i, VBO* vbo_pos, VBO* vbo_col = NULL);

};

} // namespace rendering

} // namespace cgogn

#endif // RENDERING_SHADERS_FLAT_H_
