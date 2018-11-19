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

#ifndef CGOGN_CORE_CMAP_CMAP1_BUILDER_H_
#define CGOGN_CORE_CMAP_CMAP1_BUILDER_H_

#include <cgogn/core/cmap/cmap1.h>

namespace cgogn
{

CMap1::Face add_face(CMap1& m, uint32 size);

inline void remove_face(CMap1& m, CMap1::Face f)
{
	m.remove_face_topo(f.dart);
}

CMap1::Vertex split_vertex(CMap1& m, CMap1::Vertex v);

inline void remove_vertex(CMap1& m, CMap1::Vertex v)
{
	m.remove_vertex_topo(v.dart);
}

CMap1::Vertex make_polyline(CMap1& m, uint32 size);

inline CMap1::Vertex close_hole(CMap1& m,Dart d)
{
	return CMap1::Vertex(m.close_hole_topo(d));
}

inline uint32 close_map(CMap1& m)
{
	uint32 nb_holes = 0u;
	return nb_holes;
}

template <typename FUNC>
inline void foreach_incident_vertex(CMap1& m, CMap1::Face f, const FUNC& func)
{
	static_assert(is_func_parameter_same<FUNC, CMap1::Vertex>::value, "Wrong function cell parameter type");
	m.foreach_dart_of_orbit(f, [&func] (Dart v)
	{
		return internal::void_to_true_binder(func, CMap1::Vertex(v));
	});
}

inline std::pair<CMap1::Vertex,CMap1::Vertex> vertices(const CMap1& m, CMap1::Edge e)
{
	return std::pair<CMap1::Vertex, CMap1::Vertex>(CMap1::Vertex(e.dart), CMap1::Vertex(m.phi1(e.dart)));
}



//CPP

using Vertex = CMap1::Vertex;
using Face = CMap1::Face;
using Boundary = CMap1::Boundary;
using ConnectedComponent = CMap1::ConnectedComponent;

Face add_face(CMap1& m, uint32 size)
{
	const Face f(m.add_face_topo(size));

	if (m.is_embedded<CMap1::Vertex>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			m.new_orbit_embedding(CMap1::Vertex(d));
		});
	}

	if (m.is_embedded<CMap1::Face>())
		m.new_orbit_embedding(f);
	return f;
}


Vertex split_vertex(CMap1& m, Vertex v)
{
	const Vertex nv(m.split_vertex_topo(v.dart));

	if (m.is_embedded<Vertex>())
		m.new_orbit_embedding(nv);

	if (m.is_embedded<Face>())
		m.copy_embedding<Face>(nv.dart, v.dart);

	return nv;
}

Vertex make_polyline(CMap1& m, uint32 size)
{
	const Dart v = m.add_face_topo(size);

	if (m.is_embedded<Vertex>())
	{
		m.foreach_dart_of_orbit(ConnectedComponent(v), [&m] (Dart d)
		{
			m.new_orbit_embedding(Vertex(d));
		});
	}

	m.boundary_mark(Boundary(v));

	return Vertex(m.phi1(v));
}


} //end namespace cgogn

#endif // CGOGN_CORE_CMAP_CMAP1_BUILDER_H_
