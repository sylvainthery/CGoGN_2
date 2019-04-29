
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

#ifndef CGOGN_RENDERING_PURE_GL_VIEWER_H_
#define CGOGN_RENDERING_PURE_GL_VIEWER_H_

//#include <GL/gl3w.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

#include <Eigen/Geometry>
#include <Eigen/SVD>

//#include <cgogn/rendering/cgogn_rendering_export.h>
#include <cgogn/rendering/pure_gl_viewer.h>

namespace cgogn
{
namespace rendering
{

PureGLViewer::PureGLViewer()
{}

PureGLViewer::~PureGLViewer()
{}

void PureGLViewer::close_event()
{}

void PureGLViewer::manip(MovingFrame* fr)
{
	if (fr != nullptr)
	{
		current_frame_ = fr;
		inv_cam_ = cam_.frame_.inverse();
	}
	else
	{
		current_frame_ = &(cam_.frame_);
	}
}

void PureGLViewer::key_press_event(int32 key_code)
{
	if (key_code == KeyCode::ESC)
	{
		close_event();
		exit(0);
	}
}

void PureGLViewer::key_release_event(int32 key_code)
{
}


void PureGLViewer::mouse_press_event(int32 buttons, float64 x, float64 y)
{
	if (buttons & 1)
	{
			current_frame_->is_moving_ = false;
			spinning_speed_ = 0;	
	}
	last_x = x;
	last_y = y;
}

void PureGLViewer::mouse_release_event(int32 buttons, float64 x, float64 y)
{
	if (buttons & 1)
	{
		current_frame_->is_moving_ = (spinning_speed_ > 0.05);		
	}
	last_x = x;
	last_y = y;
}


void PureGLViewer::mouse_move_event(int32 buttons, float64 x, float64 y)
{
	float64 dx = x - last_x;
	float64 dy = y - last_y;

	if (buttons & 1)
	{
		Vec3d axis(dy,dx,0.0);
		spinning_speed_ = axis_.norm();
		axis /= spinning_speed_;
		spinning_speed_ *= 0.1;
		if (obj_mode())
		{
			Transfo3d sm = Eigen::AngleAxisd(2.0*spinning_speed_,axis);
			current_frame_->spin_ = inv_cam_ *  sm * cam_.frame_;
			current_frame_->frame_ = current_frame_->spin_ * current_frame_->frame_; 
		}
		else
		{
			current_frame_->spin_ = Eigen::AngleAxisd(0.2*spinning_speed_,axis);
			current_frame_->frame_ = Eigen::AngleAxisd(spinning_speed_,axis) * current_frame_->frame_;
		}	
	}
	
	if (buttons & 2)
	{
		float64 a = cam_.scene_radius() - cam_.frame_.translation().z()/ cam_.z_cam;
		if (obj_mode())
		{
			
			float64 tx = dx / width_ * cam_.width() * a;
			float64 ty = - dy / height_ * cam_.height() * a;
			Transfo3d ntr = inv_cam_ * Eigen::Translation3d(Vec3d((tx,ty,0.0))) * cam_.frame_;
			current_frame_->frame_ = ntr * current_frame_->frame_;
		}
		else
		{
			float64 nx = float64(dx) / width_ * cam_.width() * a;
			float64 ny = - 1.0 * float64(dy) / height_ * cam_.height() * a;
			cam_.frame_.translation().x() += nx;
			cam_.frame_.translation().y() += ny;
		}
	}
}




void PureGLViewer::mouse_dbl_click(int32 buttons)
{
	if (obj_mode())
	{

	}
	else
	{

	}
}


void mouse_wheel_event(float64 dx, float64 dy)
{
	if (dy>0)
	{
		if (obj_mode())
		{
			auto ntr = inv_cam * Eigen::Translation3d(Vec3d(0,0,0.0025*delta)) * cam_.frame_);
			current_frame_ = ntr * current_frame_;
		}
		else
		{
			float64 a = cam_.s_radius - cam_.frame_.translation().z()/ cam_.z_cam/cam.s_radius;
			let ntr = 0.0025*dy*a;
			cam_.frame_.translation().z() += 0.0025*dy*a;
		}
	}
}
