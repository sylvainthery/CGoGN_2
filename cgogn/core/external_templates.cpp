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
#define CGOGN_CORE_EXTERNAL_TEMPLATES_CPP_

#include <cgogn/core/cmap/cmap2_quad.h>

namespace cgogn
{

template class CGOGN_CORE_EXPORT DartMarker<CMap2Quad>;
template class CGOGN_CORE_EXPORT DartMarkerStore<CMap2Quad>;
template class CGOGN_CORE_EXPORT DartMarkerNoUnmark<CMap2Quad>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Quad, CMap2Quad::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Quad, CMap2Quad::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Quad, CMap2Quad::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Quad, CMap2Quad::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Quad, CMap2Quad::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Quad, CMap2Quad::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Quad, CMap2Quad::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Quad, CMap2Quad::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Quad, CMap2Quad::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Quad, CMap2Quad::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Quad, CMap2Quad::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Quad, CMap2Quad::Volume::ORBIT>;


} // namespace cgogn

#include <cgogn/core/cmap/cmap2_tri.h>
namespace cgogn
{

template class CGOGN_CORE_EXPORT DartMarker<CMap2Tri>;
template class CGOGN_CORE_EXPORT DartMarkerStore<CMap2Tri>;
template class CGOGN_CORE_EXPORT DartMarkerNoUnmark<CMap2Tri>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Tri, CMap2Tri::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Tri, CMap2Tri::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Tri, CMap2Tri::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap2Tri, CMap2Tri::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Tri, CMap2Tri::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Tri, CMap2Tri::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Tri, CMap2Tri::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap2Tri, CMap2Tri::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Tri, CMap2Tri::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Tri, CMap2Tri::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Tri, CMap2Tri::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap2Tri, CMap2Tri::Volume::ORBIT>;


} // namespace cgogn

#include <cgogn/core/cmap/cmap3_hexa.h>

namespace cgogn
{

template class CGOGN_CORE_EXPORT DartMarker<CMap3Hexa>;
template class CGOGN_CORE_EXPORT DartMarkerStore<CMap3Hexa>;
template class CGOGN_CORE_EXPORT DartMarkerNoUnmark<CMap3Hexa>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Hexa, CMap3Hexa::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Hexa, CMap3Hexa::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Hexa, CMap3Hexa::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Hexa, CMap3Hexa::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Hexa, CMap3Hexa::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Hexa, CMap3Hexa::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Hexa, CMap3Hexa::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Hexa, CMap3Hexa::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Hexa, CMap3Hexa::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Hexa, CMap3Hexa::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Hexa, CMap3Hexa::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Hexa, CMap3Hexa::Volume::ORBIT>;

} // namespace cgogn

#include <cgogn/core/cmap/cmap3_tetra.h>

namespace cgogn
{

template class CGOGN_CORE_EXPORT DartMarker<CMap3Tetra>;
template class CGOGN_CORE_EXPORT DartMarkerStore<CMap3Tetra>;
template class CGOGN_CORE_EXPORT DartMarkerNoUnmark<CMap3Tetra>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Tetra, CMap3Tetra::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Tetra, CMap3Tetra::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Tetra, CMap3Tetra::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarker<CMap3Tetra, CMap3Tetra::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Tetra, CMap3Tetra::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Tetra, CMap3Tetra::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Tetra, CMap3Tetra::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerNoUnmark<CMap3Tetra, CMap3Tetra::Volume::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Tetra, CMap3Tetra::Vertex::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Tetra, CMap3Tetra::Edge::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Tetra, CMap3Tetra::Face::ORBIT>;
template class CGOGN_CORE_EXPORT CellMarkerStore<CMap3Tetra, CMap3Tetra::Volume::ORBIT>;

} // namespace cgogn



//namespace cgogn
//{
////template class CGOGN_CORE_EXPORT CMap3Builder_T<CMap3Hexa>; // TODO : fix compilation
////template class CGOGN_CORE_EXPORT CMap3Builder_T<CMap3Tetra>;// TODO : fix compilation
//} // namespace cgogn


