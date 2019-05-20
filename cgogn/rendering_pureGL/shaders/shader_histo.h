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

#ifndef CGOGN_RENDERING_SHADERS_HISTO_H_
#define CGOGN_RENDERING_SHADERS_HISTO_H_
#include <GL/gl3w.h>
#include <cgogn/rendering_pureGL/cgogn_rendering_puregl_export.h>
#include <cgogn/rendering_pureGL/shaders/shader_program.h>
#include <cgogn/rendering_pureGL/texture.h>
#include <cgogn/rendering_pureGL/fbo.h>

namespace cgogn
{

namespace rendering_pgl
{
DECLARE_SHADER_CLASS(Histo)

class CGOGN_RENDERING_PUREGL_EXPORT ShaderParamHisto : public ShaderParam
{
	inline void set_uniforms() override
	{
        shader_->set_uniforms_values(texture_->bind(0), texture_->width(),1.0f);
	}

public:
	Texture2D* tex_fbo_;
	FBO* fbo_;
	Texture2D* texture_;

	using LocalShader = ShaderHisto;

	inline ShaderParamHisto(LocalShader* sh) :
		ShaderParam(sh)
	{
		tex_fbo_ = new Texture2D();
		tex_fbo_->alloc(1,1,GL_R32F,GL_RED,nullptr,GL_FLOAT);
		fbo_ = new FBO({tex_fbo_},false,nullptr);
	}

	inline ~ShaderParamHisto() override {}

	inline void draw(int nbb, std::vector<float>& histogram)
	{
		histogram.resize(nbb);

		fbo_->resize(nbb,1);

		bind();
		shader_->set_uniform_value(2,1.0f-0.5f/nbb);
		fbo_->bind();

		GLenum idbuf = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1,&idbuf);
//		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClearColor(0.0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
		glDrawArrays(GL_POINTS,0, texture_->width()*texture_->height());
		glDisable(GL_BLEND);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0,0,nbb,1,GL_RED,GL_FLOAT,histogram.data());		fbo_->release();
		release();

		for( float h: histogram)
			std::cout << "|" << h ;
		float tot=0;
		for( float h: histogram)
			tot+=h;
		std::cout << "| => " << tot << std::endl;

	}

};

} // namespace rendering_pgl
} // namespace cgogn

#endif
