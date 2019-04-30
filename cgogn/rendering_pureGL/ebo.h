﻿
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

#ifndef CGOGN_RENDERING_EBO_H_
#define CGOGN_RENDERING_EBO_H_

#include <GL/gl3w.h>
#include <string>
#include <cgogn/core/utils/numerics.h>
#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>

namespace cgogn
{

namespace rendering_pgl
{

class CGOGN_RENDERING_PUREGL_EXPORT EBO
{
protected:
	GLuint id_;
	std::size_t nb_;
public:

	inline EBO() :
		id_(0), nb_(0)
	{}

	inline void create()
	{
		glGenBuffers(1, &id_);
	}

	inline bool is_created()
	{
		return id_ != 0;
	}

	inline ~EBO()
	{
		glDeleteBuffers(1,&id_);
	}

	inline void bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
	}

	inline void release()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	inline void allocate(std::size_t nb_ind)
	{
		if (nb_ind != nb_) // only allocate when > ?
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(nb_ind*sizeof(GLuint)), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			nb_ = nb_ind;
		}
	}

	inline void allocate(GLuint* indices, std::size_t nb_ind)
	{
		if (nb_ind != nb_) // only allocate when > ?
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(nb_ind*sizeof(GLuint)), indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			nb_ = nb_ind;
		}
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, GLsizeiptr(nb_ind*sizeof(GLuint)), indices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	/**
	 * @brief get and lock pointer on buffer memory
	 * @return  the pointer
	 */
	inline GLuint* lock_pointer()
	{
		this->bind();
		return reinterpret_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_ONLY));
	}

	/**
	 * @brief release_pointer
	 */
	inline void release_pointer()
	{
		this->bind();
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		this->release();
	}

	/**
	 * @brief copy data
	 * @param offset offset in bytes in the bufffer
	 * @param nb number of bytes to copy
	 * @param src source pointer
	 */
	inline void copy_data(uint32 offset, std::size_t nb, const void* src)
	{
		this->bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, GLsizeiptr(nb), src);
		this->release();
	}

	uint32 size() const
	{
		return uint32(nb_);
	}

	GLuint id() const
	{
		return id_;
	}
};

} // namespace rendering_pgl

} // namespace cgogn

#endif // CGOGN_RENDERING_EBO_H_
