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

#include <cgogn/core/cmap/cmap2_tri.h>

namespace cgogn
{

// local CPP using
using CDart  = CMap2Tri::CDart;
using Vertex = CMap2Tri::Vertex;
using Edge   = CMap2Tri::Edge;
using Face   = CMap2Tri::Face;
using Volume = CMap2Tri::Volume;

Face add_face(CMap2Tri& m, uint32 size)
{
	cgogn_message_assert(size == 3u, "Can create only triangles");
	if (size != 3)
		cgogn_log_warning("add_face") << "Attempt to create a face which is not a triangles";

	const Face f(m.add_tri_topo());

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


Volume add_tetra(CMap2Tri& m)
{
	Volume vol(m.add_tetra_topo());

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

void flip_edge(CMap2Tri& m, Edge e)
{
	if (m.flip_edge_topo(e.dart))
	{
		Dart d = e.dart;
		Dart dd = m.phi2(d);

		Dart d1  = m.phi1(d);
		Dart d11 = m.phi1(d1);
		Dart dd1  = m.phi1(dd);
		Dart dd11 = m.phi1(dd1);

		if (m.is_embedded<Vertex>())
		{
			// warning not all darts of boundary are embedded
			// only those which are m.phi2-sewed with non-boundary
			m.copy_embedding<Vertex>(d1, m.phi2(dd11));
			m.copy_embedding<Vertex>(dd1, m.phi2(d11));
			m.copy_embedding<Vertex>(d11, m.phi2(d1));
			m.copy_embedding<Vertex>(dd11, m.phi2(dd1));
			m.copy_embedding<Vertex>(d, dd1);
			m.copy_embedding<Vertex>(dd, d1);

		}

		if (m.is_embedded<Edge>())
		{
			m.copy_embedding<Edge>(d1, m.phi2(d1));
			m.copy_embedding<Edge>(d11, m.phi2(d11));
			m.copy_embedding<Edge>(dd1, m.phi2(dd1));
			m.copy_embedding<Edge>(dd11, m.phi2(dd11));
		}
	}
}

Vertex collapse_edge(CMap2Tri& m, Edge e)
{
	// dart for edge of one side
	Dart d2 = m.phi2(m.phi_1(m.phi2(e.dart)));
	// dart for edge of other side, and vertex
	Dart d1 = m.collapse_edge_topo(e.dart);

	Vertex v(d1);

	if (m.is_embedded<Vertex>())
		m.set_orbit_embedding<Vertex>(v, m.embedding(v));

	if (m.is_embedded<Edge>())
	{
		m.copy_embedding<Edge>(d1, m.phi2(d1));
		m.copy_embedding<Edge>(d2, m.phi2(d2));
	}

	return v;
}

Edge split_vertex(CMap2Tri& m, Dart d1, Dart d2)
{
	Edge res_edge(m.split_vertex_topo(d1,d2));

	if (m.is_embedded<CDart>())
	{
		foreach_incident_face(m, res_edge, [&m] (Face nf)
		{
			m.foreach_dart_of_orbit(nf, [&m] (Dart d)
			{
				m.new_orbit_embedding(CDart(d));
			});
		});
	}

	if (m.is_embedded<Vertex>())
	{
		m.new_orbit_embedding(Vertex(res_edge.dart));
		Dart vd1 = m.phi2(res_edge.dart);
		Dart d0 = m.phi2(m.phi_1(vd1));
		m.copy_embedding<Vertex>(vd1, d0);
		Dart vd2 = m.phi1(res_edge.dart);
		m.copy_embedding<Vertex>(vd2, d0);

		foreach_incident_face(m,res_edge, [&m] (Face nf)
		{
			Dart dv1 = m.phi_1(nf.dart); // new dart of exiting vertex
			Dart dv0 = m.phi2(m.phi1(nf.dart)); // old dart of exiting vertex
			m.copy_embedding<Vertex>(dv1, dv0);
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.new_orbit_embedding(res_edge); // new edge
		foreach_incident_face(m,res_edge, [&m] (Face nf)
		{
			Dart d = m.phi1(nf.dart);
			m.new_orbit_embedding(Edge(d));
			d = m.phi1(d);
			m.copy_embedding<Edge>(d, m.phi2(d)); // more efficient to use the old emb
		});
	}

	if (m.is_embedded<Face>())
	{
		foreach_incident_face(m,res_edge, [&m] (Face nf)
		{
			m.new_orbit_embedding(nf);
		});
	}

	if (m.is_embedded<Volume>())
	{
		uint32 emb = m.embedding(Volume(m.phi<12>(res_edge.dart)));
		foreach_incident_face(m,res_edge, [&m, emb] (Face nf)
		{
			m.set_orbit_embedding<Volume>(nf, emb);
		});
	}

	return res_edge;
}

Vertex cut_edge(CMap2Tri& m, Edge e)
{
	Vertex nv(m.cut_edge_topo(e));

	if (m.is_embedded<CDart>())
	{
		foreach_incident_face(m, nv, [&m] (Face nf)
		{
			m.foreach_dart_of_orbit(nf, [&m] (Dart d)
			{
				m.new_orbit_embedding(CDart(d));
			});
		});
	}

	if (m.is_embedded<Vertex>())
	{
		m.new_orbit_embedding(Vertex(nv));

		foreach_incident_edge(m, nv, [&m] (Edge ne)
		{
			Dart v1 = m.phi2(ne.dart); // new dart of existing vertex
			Dart v2 = m.phi1(ne.dart); // new dart of existing vertex
			Dart v0 = m.phi2(m.phi_1(v1)); // old dart of existing vertex
			m.copy_embedding<Vertex>(v1,v0);
			m.copy_embedding<Vertex>(v2,v0);
		});
	}

	if (m.is_embedded<Edge>())
	{
		foreach_incident_edge(m,nv, [&m] (Edge ne)
		{
			m.new_orbit_embedding(ne); // new edge
			Dart ne2 = m.phi1(ne.dart); // new dart of existing edge
			m.copy_embedding<Edge>(ne2, m.phi2(ne2));
		});
	}

	if (m.is_embedded<Face>())
	{
		foreach_incident_face(m, nv, [&m] (Face nf)
		{
			m.new_orbit_embedding(nf);
		});
	}

	if (m.is_embedded<Volume>())
	{
		uint32 emb = m.embedding(Volume(m.phi<12>(nv.dart)));
		foreach_incident_face(m, nv, [&m, emb] (Face nf)
		{
			m.set_orbit_embedding<Volume>(nf, emb);
		});
	}

	return nv;
}

Vertex split_triangle(CMap2Tri& m, Face f)
{
	Vertex vc = m.split_triangle_topo(f);

	if (m.is_embedded<CDart>())
	{
		foreach_incident_face(m, vc, [&m] (Face nf)
		{
			m.foreach_dart_of_orbit(nf, [&m] (Dart d)
			{
				m.new_orbit_embedding(CDart(d));
			});
		});
	}

	if (m.is_embedded<Vertex>())
	{
		m.new_orbit_embedding(Vertex(vc));

		foreach_incident_edge(m, vc, [&m] (Edge ne)
		{
			Dart v1 = m.phi2(ne.dart); // new dart of exiting vertex
			Dart v2 = m.phi1(ne.dart); // new dart of exiting vertex
			Dart v0 = m.phi2(m.phi_1(v1)); // old dart of exiting vertex
			m.copy_embedding<Vertex>(v1,v0);
			m.copy_embedding<Vertex>(v2,v0);
		});
	}

	if (m.is_embedded<Edge>())
	{
		foreach_incident_edge(m, vc, [&m] (Edge ne)
		{
			m.new_orbit_embedding(ne); // new edge
			Dart ne2 = m.phi1(ne.dart); // new dart of existing dart
			m.copy_embedding<Edge>(ne2, m.phi2(ne2));
		});
	}

	if (m.is_embedded<Face>())
	{
		foreach_incident_face(m, vc, [&m] (Face nf)
		{
			m.new_orbit_embedding(nf);
		});
	}

	if (m.is_embedded<Volume>())
	{
		uint32 emb = m.embedding(Volume(m.phi<12>(vc.dart)));
		foreach_incident_face(m, vc, [&m, emb] (Face nf)
		{
			m.set_orbit_embedding<Volume>(nf, emb);
		});
	}

	return vc;
}



Face close_hole(CMap2Tri& m, Dart d)
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

uint32 close_map(CMap2Tri& m)
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
			Vertex fan_center(m.phi_1(f.dart));
			foreach_incident_face(m, fan_center, [&] (Face ff)
			{
				m.boundary_mark(ff);
			});
			++nb_holes;
		}
	}

	cgogn::dart_buffers()->release_buffer(fix_point_darts);

	return nb_holes;
}


} // namespace cgogn


