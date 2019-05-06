
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

#ifndef CGOGN_RENDERING_FBO_H_
#define CGOGN_RENDERING_FBO_H_

#include <GL/gl3w.h>
#include <vector>
#include <cgogn/core/utils/numerics.h>
#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/texture.h>

namespace cgogn
{

namespace rendering_pgl
{

class CGOGN_RENDERING_PUREGL_EXPORT FBO
{
public:
	FBO(std::vector<Texture2D*> textures, bool add_depth, FBO* from );

	inline void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id_);
	}

	inline static void release()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void resize(int w, int h);

protected:
	GLuint id_;
	GLuint depth_render_buffer_;
	std::vector<Texture2D*> tex_;
};

}
}
#endif
