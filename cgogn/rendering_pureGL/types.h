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

#ifndef CGOGN_RENDERING_TYPE_H_
#define CGOGN_RENDERING_TYPE_H_

#include <GL/gl3w.h>
#include <cgogn/core/utils/numerics.h>
#include <cgogn/geometry/types/eigen.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Geometry>
#include <Eigen/SVD>


namespace cgogn
{
namespace rendering_pgl
{
using Vec2d = Eigen::Vector2d;
using Vec3d = Eigen::Vector3d;
using Mat3d = Eigen::Matrix3d;
using Mat4d = Eigen::Matrix4d;
using Mat4f = Eigen::Matrix4f;
using Transfo3d = Eigen::Affine3d;
using GLMat4  = Eigen::Matrix4f;
using GLMat3  = Eigen::Matrix3f;
using GLColor = Eigen::Vector4f;
using GLVec2  = Eigen::Vector2f;
using GLVec3  = Eigen::Vector3f;
using GLVec4  = Eigen::Vector4f;


inline GLColor col4i(uint8 R, uint8 G, uint8 B, uint8 A)
{
	return GLColor(R/255.0f, G/255.0f, B/255.0f, A/255.0f);
}

inline GLColor col3i(uint8 R, uint8 G, uint8 B)
{
	return GLColor(R/255.0f, G/255.0f, B/255.0f, 1.0f);
}

inline GLColor col1i(uint8 R)
{
	return GLColor(R/255.0f, R/255.0f, R/255.0f, 1.0f);
}



} // namespace rendering_pgl
} // namespace cgogn



#endif // CGOGN_RENDERING_SHADERS_SHADERPROGRAM_H_
