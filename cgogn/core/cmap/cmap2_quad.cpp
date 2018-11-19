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

#include <cgogn/core/cmap/cmap2_quad.h>

namespace cgogn
{

// local CPP using
using CDart  = CMap2Quad::CDart;
using Vertex = CMap2Quad::Vertex;
using Edge   = CMap2Quad::Edge;
using Face   = CMap2Quad::Face;
using Volume = CMap2Quad::Volume;

Face add_face(CMap2Quad& m, uint32 size)
{
	cgogn_message_assert(size == 4u, "Can create only quad");
	if (size != 4)
		cgogn_log_warning("add_face") << "Attempt to create a face which is not a quad";

	const Face f(m.add_quad_topo());

	if (m.is_embedded<CDart>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			m.new_orbit_embedding(CDart(d));
		});
	}

	if (m.is_embedded<Vertex>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			m.new_orbit_embedding(Vertex(d));
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			m.new_orbit_embedding(Edge(d));
		});
	}

	if (m.is_embedded<Face>())
		m.new_orbit_embedding(f);

	if (m.is_embedded<Volume>())
		m.new_orbit_embedding(Volume(f.dart));

	return f;
}

Volume add_hexa(CMap2Quad& m)
{
	Volume vol(m.add_hexa_topo());

	if (m.is_embedded<CDart>())
	{
		m.foreach_dart_of_orbit(vol, [&m] (Dart d)
		{
			m.new_orbit_embedding(CDart(d));
		});
	}

	if (m.is_embedded<Vertex>())
	{
		foreach_incident_vertex(m, vol, [&m] (Vertex v)
		{
			m.new_orbit_embedding(v);
		});
	}

	if (m.is_embedded<Edge>())
	{
		foreach_incident_edge(m, vol, [&m] (Edge e)
		{
			m.new_orbit_embedding(e);
		});
	}

	if (m.is_embedded<Face>())
	{
		foreach_incident_face(m, vol, [&m] (Face f)
		{
			m.new_orbit_embedding(f);
		});
	}

	if (m.is_embedded<Volume>())
		m.new_orbit_embedding(vol);

	return vol;
}


Face extrude_quad(CMap2Quad& m, Face f)
{
	m.extrude_quad_topo(f);

	if (m.is_embedded<CDart>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			// darts of f
			m.new_orbit_embedding(CDart(d));
			// darts of faces adjacent to f
			m.foreach_dart_of_orbit(Face(m.phi2(d)), [&m] (Dart e)
			{
				m.new_orbit_embedding(CDart(e));
			});
		});
	}

	if (m.is_embedded<Vertex>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			m.new_orbit_embedding(Vertex(d));
			Dart v1 = m.phi_1(m.phi2(d));
			Dart v0 = m.phi2(m.phi_1(v1));
			m.copy_embedding<Vertex>(v1, v0);
			Dart v2 = m.phi1(m.phi2(v1));
			m.copy_embedding<Vertex>(v2, v0);
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart d)
		{
			m.new_orbit_embedding(Edge(d));
			Dart d1 = m.phi1(m.phi2(d));
			m.new_orbit_embedding(Edge(d1));
			d1 = m.phi1(d1);
			m.copy_embedding<Edge>(d1, m.phi2(d1));
		});
	}

	if (m.is_embedded<Face>())
	{
		m.new_orbit_embedding(f);
		foreach_adjacent_face_through_edge(m, f, [&m] (Face fi)
		{
			m.new_orbit_embedding(fi);
		});
	}

	if (m.is_embedded<Volume>())
	{
		uint32 emb = m.embedding(Volume(m.phi<2112>(f.dart)));
		m.set_orbit_embedding<Volume>(f, emb);
		foreach_adjacent_face_through_edge(m, f, [&m, emb] (Face fi)
		{
			m.set_orbit_embedding<Volume>(fi, emb);
		});
	}

	return f;
}


Face close_hole(CMap2Quad& m, Dart d)
{
	//	const Face f(map_.close_hole_topo(d));
	Dart dh = m.close_hole_topo(d);

	Dart di = dh;

	do
	{
		Dart di0 = m.phi2(di);
		Dart di1 = m.phi1(di);

		if (m.is_embedded<Vertex>())
		{
			m.copy_embedding<Vertex>(di, m.phi1(di0));
			m.copy_embedding<Vertex>(di1, di0);
		}

		if (m.is_embedded<Edge>())
			m.copy_embedding<Edge>(di, di0);

		if (m.is_embedded<Volume>())
			m.set_orbit_embedding<Volume>(Face(di), m.embedding(Volume(d)));

		di = m.phi<21>(di1);
	} while (di != dh);

	return Face(dh);
}

/**
 * @brief close_map
 * @return the number of holes (filled)
 */
uint32 close_map(CMap2Quad& m)
{
	uint32 nb_holes = 0;

	std::vector<Dart>* fix_point_darts = cgogn::dart_buffers()->buffer();
	m.foreach_dart([&] (Dart d)
	{
		if (m.phi2(d) == d)
			fix_point_darts->push_back(d);
	});

	for (Dart d : (*fix_point_darts))
	{
		if (m.phi2(d) == d)
		{
			Face f = close_hole(m,d);
			Dart df = f.dart;
			do
			{
				m.boundary_mark(Face(df));
				df = m.phi<121>(df);
			} while (df != f.dart);
			++nb_holes;
		}
	}
	cgogn::dart_buffers()->release_buffer(fix_point_darts);
	return nb_holes;
}



} // namespace cgogn


