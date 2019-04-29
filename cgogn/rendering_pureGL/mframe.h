
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

#ifndef CGOGN_RENDERING_CAMERA_H_
#define CGOGN_RENDERING_CAMERA_H_

//#include <GL/gl3w.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

#include <Eigen/Geometry>
#include <Eigen/SVD>

//#include <cgogn/rendering/cgogn_rendering_export.h>
#include <cgogn/core/utils/numerics.h>

namespace cgogn
{
namespace rendering
{
using Vec2d = Eigen::Vector2d;
using Vec3d = Eigen::Vector3d;
using Mat3d = Eigen::Matrix3d;
using Mat4d = Eigen::Matrix4d;
using Mat4f = Eigen::Matrix4f;
using Transfo3d = Eigen::Affine3d;


struct MovingFrame
{
	Transfo3d frame_;
	Transfo3d spin_;
	bool is_moving_;

	MovingFrame():
		frame_(Transfo3d::Identity()),
		spin_(Transfo3d::Identity()),
		is_moving_(false)
	{}

	//Vec3d local_coordinates(Vec3d glob);
};

class /*CGOGN_RENDERING_EXPORT*/ Camera : public MovingFrame
{
public:
	enum Type { PERSPECTIVE, ORTHOGRAPHIC };
private:
	Type type_;
	float64 field_of_view_;
	float64 asp_ratio_; // width/height
	Vec3d scene_center_;
	float64 scene_radius_;

	inline Mat4d perspective(float64 near, float64 far) const
	{
		float64 range_inv = 1.0 / (near - far);
		float64 f = 1.0/std::tan(field_of_view_/2.0);
		auto m05 = (asp_ratio_<1) ? std::make_pair(f/asp_ratio_,f) : std::make_pair(f,f*asp_ratio_);
		Mat4d m;
		m << m05.first,  0,  0,  0,
			  0, m05.second,  0,  0,
			  0,  0, (near+far)*range_inv, -1,
			  0,  0, 2.0*near*far*range_inv,0;
		return m;
	}

	inline Mat4d ortho(float64 near, float64 far) const
	{
		float64 range_inv = 1.0 / (near - far);
		auto m05 = (asp_ratio_<1) ? std::make_pair(1.0/asp_ratio_,1.0) : std::make_pair(1.0,1.0/asp_ratio_);
		Mat4d m;
		m << m05.first,  0,  0,  0,
			  0, m05.second,  0,  0,
			  0,  0, 2*range_inv, 0,
			  0,  0, (near+far)*range_inv,0;
		return m;
	}


public:
	inline Camera():
		type_(PERSPECTIVE),
		field_of_view_(1.57),
		asp_ratio_(1.0)
	{}
	inline float64 width() const { return (asp_ratio_>1.0) ? asp_ratio_ : 1.0;}
	
	inline float64 height() const { return (asp_ratio_>1.0) ? 1.0 : 1.0/asp_ratio_;}
	
	inline void set_type(Type type) { type_ = type; }
	
	inline void set_field_of_view(float64 fov) { field_of_view_ = fov; }
	
	inline void set_aspect_ratio(float64 aspect) { asp_ratio_ = aspect; }
	
	inline void set_scene_radius(float64 radius) { scene_radius_ = radius; }
	
	inline void set_scene_center(const Vec3d& center) {scene_center_ = center; }

	inline void center_scene() { this->frame_.matrix().block<3,1>(0,3).setZero(); }
	
	inline void show_entire_scene() 
	{
		this->frame_.matrix().block<3,1>(0,0).normalize();
		this->frame_.matrix().block<3,1>(0,1).normalize();
		this->frame_.matrix().block<3,1>(0,2).normalize();
	}

	inline float64 scene_radius() const { return scene_radius_; }
	
	inline const Vec3d& scene_center() const { return scene_center_; }

	void look_at(const Vec3d&

	inline Mat4f get_projection_matrix() const
	{
		float64 d = scene_radius_/std::tan(field_of_view_) - this->frame_.coeff(3,2);
		float64 znear = std::max(0.001, d - scene_radius_);
		float64 zfar = d+scene_radius_;
		return ((type_==PERSPECTIVE) ? perspective(znear,zfar) : ortho(znear,zfar)).cast<float>();
	}

	Mat4f get_modelview_matrix() const
	{
		Transfo3d m = Eigen::Translation3d(Vec3d(0.0,0.0,-scene_radius_/std::tan(field_of_view_))) * this->frame_ * Eigen::Translation3d(-scene_center_);
		return m.matrix().cast<float32>();
	}

};



class BaseViewer
{
	Camera cam_;
	MovingFrame* current_frame_;
	Transfo3d inv_cam_;
	int32 width;
	int32 height;
	float64 speed_;

public:
	BaseViewer();

	inline bool obj_mode() const { return  current_frame_ != &(cam_.frame_);}

	void manip(MovingFrame* fr);

	void mouse_up(int32 button, int32 x, int32 y);
	void mouse_down(int32 button, int32 x, int32 y);
	void mouse_move(int32 button, int32 moveX, int32 moveY)
	void mouse_dbl_click(int32 button, int32 x, int32 y);
	void mouse_wheel(int32 dd);
};

BaseViewer::BaseViewer()
{}

void BaseViewer::manip(MovingFrame* fr)
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

void BaseViewer::key_up(int32 code)
{
}

void BaseViewer::key_down(int32 code)
{
}


void BaseViewer::mouse_up(int32 button, int32 x, int32 y)
{
	switch(button)
	{
		case 0:
		{
			current_frame_->is_moving_ = (speed_ > 0.05);		
		}
		break;
		case 1:
		{
			
		}
		break;
		case 2:
		{
			
		}
		break;
	}
}

void BaseViewer::mouse_down(int32 button, int32 x, int32 y)
{
	switch(button)
	{
		case 0:
		{
			current_frame_->is_moving_ = false;
			speed_ = 0;	
		}
		break;
		case 1:
		{
			
		}
		break;
		case 2:
		{
			
		}
		break;
	}
}


void BaseViewer::mouse_move(int32 button, int32 moveX, int32 moveY)
{
	switch(button)
	{
		case 0:
		{
			Vec3d axis(moveY,moveX,0.0);
			speed_ = axis_.norm();
			axis /= speed_;
			speed_ *= 0.1;
			if (obj_mode())
			{
				Transfo3d sm = Eigen::AngleAxisd(2.0*speed_,axis);
				current_frame_->spin_ = inv_cam_ *  sm * cam_.frame_;
				current_frame_->frame_ = current_frame_->spin_ * current_frame_->frame_; // mult3
			}
			else
			{
				current_frame_->spin_ = Eigen::AngleAxisd(0.2*speed_,axis);
				current_frame_->frame_ = Eigen::AngleAxisd(speed_,axis) * current_frame_->frame_; // mult3
			}	
		}
		break;
		case 1:
		{
			float64 a = cam_.scene_radius() - cam_.frame_.translation().z()/ cam_.z_cam;
			if (obj_mode())
			{
				
				float64 tx = moveX / width_ * cam_.width() * a;
				float64 ty = - moveY / height_ * cam_.height() * a;
				Transfo3d ntr = inv_cam_ * Eigen::Translation3d(Vec3d((tx,ty,0.0))) * cam_.frame_;
				current_frame_->frame_ = ntr * current_frame_->frame_;
			}
			else
			{
				float64 nx = float64(moveX) / width_ * cam_.width() * a;
				float64 ny = - 1.0 * float64(moveY) / height_ * cam_.height() * a;
				cam_.frame_.translation().x() += nx;
				cam_.frame_.translation().y() += ny;
			}
		}
		break;
		case 2:
		{
			
		}
		break;
	}
}




void BaseViewer::mouse_dbl_click(int32 button)
{
	if (obj_mode())
	{

	}
	else
	{

	}
}


void wheel()
{
	if (obj_mode())
	{

	}
	else
	{

	}
}




}
}
#endif
