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

#include <cgogn/core/cmap/cmap2.h>

namespace cgogn
{

using CDart  = CMap2::CDart;
using Vertex = CMap2::Vertex;
using Edge   = CMap2::Edge;
using Face   = CMap2::Face;
using Volume = CMap2::Volume;


Face add_face(CMap2& m, uint32 size)
{
	const Face f(m.add_face_topo(size));

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


Volume add_pyramid(CMap2& m, std::size_t size)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	const Volume vol(m.add_pyramid_topo(size));

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

Volume add_prism(CMap2& m, std::size_t size)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	const Volume vol(m.add_prism_topo(size));

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

inline Vertex cut_edge(CMap2& m, Edge e)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	const Dart v = m.cut_edge_topo(e.dart);
	const Dart nf = m.phi2(e.dart);
	const Dart f = m.phi2(v);

	if (m.is_embedded<CDart>())
	{
		if (!m.is_boundary_cell(CDart(v))) m.new_orbit_embedding(CDart(v));
		if (!m.is_boundary_cell(CDart(nf))) m.new_orbit_embedding(CDart(nf));
	}

	if (m.is_embedded<Vertex>())
		m.new_orbit_embedding(Vertex(v));

	if (m.is_embedded<Edge>())
	{
		m.copy_embedding<Edge>(nf, e.dart);
		m.new_orbit_embedding(Edge(v));
	}

	if (m.is_embedded<Face>())
	{
		if (!m.is_boundary_cell(Face(e.dart)))
			m.copy_embedding<Face>(v, e.dart);
		if (!m.is_boundary_cell(Face(f)))
			m.copy_embedding<Face>(nf, f);
	}

	if (m.is_embedded<Volume>())
	{
		m.copy_embedding<Volume>(v, e.dart);
		m.copy_embedding<Volume>(nf, e.dart);
	}

	return Vertex(v);
}


inline void flip_edge(CMap2& m, Edge e)
{
	if (m.flip_edge_topo(e.dart))
	{
		Dart d = e.dart;
		Dart d2 = m.phi2(d);

		if (m.is_embedded<Vertex>())
		{
			m.copy_embedding<Vertex>(d, m.phi1(d2));
			m.copy_embedding<Vertex>(d2, m.phi1(d));
		}

		if (m.is_embedded<Face>())
		{
			m.copy_embedding<Face>(m.phi_1(d), d);
			m.copy_embedding<Face>(m.phi_1(d2), d2);
		}
	}
}

inline void flip_back_edge(CMap2& m, Edge e)
{
	if (m.flip_back_edge_topo(e.dart))
	{
		const Dart d = e.dart;
		const Dart d2 = m.phi2(d);

		if (m.is_embedded<Vertex>())
		{
			m.copy_embedding<Vertex>(d, m.phi1(d2));
			m.copy_embedding<Vertex>(d2, m.phi1(d));
		}

		if (m.is_embedded<Face>())
		{
			m.copy_embedding<Face>(m.phi1(d), d);
			m.copy_embedding<Face>(m.phi1(d2), d2);
		}
	}
}


bool edge_can_collapse(CMap2& m, Edge e)
{
	auto v = vertices(m,e);

	if (m.is_incident_to_boundary(v.first) || m.is_incident_to_boundary(v.second))
		return false;

	uint32 val_v1 = m.degree(v.first);
	uint32 val_v2 = m.degree(v.second);

	if (val_v1 + val_v2 < 8 || val_v1 + val_v2 > 14)
		return false;

	Dart e1 = e.dart;
	Dart e2 = m.phi2(e.dart);

	if (m.codegree(Face(e1)) == 3)
	{
		if (m.degree(Vertex(m.phi_1(e1))) < 4)
			return false;
	}

	if (m.codegree(Face(e2)) == 3)
	{
		if (m.degree(Vertex(m.phi_1(e2))) < 4)
			return false;
	}

	auto next_edge = [&m] (Dart d) { return m.phi2(m.phi_1(d)); };

	// Check vertex sharing condition
	std::vector<uint32>* vn1 = uint_buffers()->buffer();
	Dart it = next_edge(next_edge(e1));
	Dart end = m.phi1(e2);
	do
	{
		vn1->push_back(m.embedding(Vertex(m.phi1(it))));
		it = next_edge(it);
	} while(it != end);
	it = next_edge(next_edge(e2));
	end = m.phi1(e1);
	do
	{
		auto vn1it = std::find(vn1->begin(), vn1->end(), m.embedding(Vertex(m.phi1(it))));
		if (vn1it != vn1->end())
			return false;
		it = next_edge(it);
	} while(it != end);
	uint_buffers()->release_buffer(vn1);

	return true;
}

/**
 * @brief Collapse an edge
 * @param e : the edge to collapse
 * @return the resulting vertex
 */
inline Vertex collapse_edge(CMap2& m, Edge e)
{
	Dart e1 = m.phi2(m.phi_1(e.dart));
	Dart e2 = m.phi2(m.phi_1(m.phi2(e.dart)));

	Vertex v(m.collapse_edge_topo(e.dart));

	if (m.is_embedded<Vertex>())
		m.set_orbit_embedding<Vertex>(v, m.embedding(v));

	if (m.is_embedded<Edge>())
	{
		m.copy_embedding<Edge>(m.phi2(e1), e1);
		m.copy_embedding<Edge>(m.phi2(e2), e2);
	}

	return v;
}

inline void split_vertex(CMap2& m, Dart d, Dart e)
{
	const Dart dd = m.phi2(d) ;
	const Dart ee = m.phi2(e) ;

	m.split_vertex_topo(d, e) ;

	if (m.is_embedded<CDart>())
	{
		m.new_orbit_embedding(CDart(m.phi1(dd)));
		m.new_orbit_embedding(CDart(m.phi1(ee)));
	}

	if (m.is_embedded<Vertex>())
	{
		m.new_orbit_embedding(Vertex(e));
		m.copy_embedding<Vertex>(m.phi1(dd), d);
	}

	if (m.is_embedded<Edge>())
	{
		m.new_orbit_embedding(Edge(m.phi1(dd)));
	}

	if (m.is_embedded<Face>())
	{
		m.copy_embedding<Face>(m.phi1(dd), dd);
		m.copy_embedding<Face>(m.phi1(ee), ee);
	}
}


void merge_incident_edges(CMap2& m, Vertex v)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	Dart d = m.phi_1(v.dart);
	if (m.merge_incident_edges_topo(v.dart))
	{
		if (m.is_embedded<Edge>())
			m.copy_embedding<Edge>(m.phi2(d), d);
	}
}


void merge_incident_faces(CMap2& m, Edge e)
{
	Dart d1 = m.phi1(e.dart);
	if (m.merge_incident_faces_of_edge_topo(e.dart))
	{
		if (m.is_embedded<Face>())
			m.set_orbit_embedding<Face>(Face(d1), m.embedding(Face(d1)));
	}
}


void merge_incident_faces(CMap2& m, Vertex v)
{
	Dart d1 = m.phi1(v.dart);
	if (m.merge_incident_faces_of_vertex_topo(v.dart))
	{
		if (m.is_embedded<Face>())
			m.set_orbit_embedding<Face>(Face(d1), m.embedding(Face(d1)));
	}
}


inline Edge cut_face(CMap2& m, Dart d, Dart e)
{
	cgogn_message_assert(!m.is_boundary_cell(Face(d)), "cut_face: should not cut a boundary face");

	Dart nd = m.cut_face_topo(d, e);
	Dart ne = m.phi_1(e);

	if (m.is_embedded<CDart>())
	{
		m.new_orbit_embedding(CDart(nd));
		m.new_orbit_embedding(CDart(ne));
	}

	if (m.is_embedded<Vertex>())
	{
		m.copy_embedding<Vertex>(nd, e);
		m.copy_embedding<Vertex>(ne, d);
	}

	if (m.is_embedded<Edge>())
		m.new_orbit_embedding(Edge(nd));

	if (m.is_embedded<Face>())
	{
		m.copy_embedding<Face>(nd, d);
		m.new_orbit_embedding(Face(ne));
	}

	if (m.is_embedded<Volume>())
	{
		m.copy_embedding<Volume>(nd, d);
		m.copy_embedding<Volume>(ne, d);
	}

	return Edge(nd);
}


inline void unsew_faces(CMap2& m, Edge e)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	const Dart d = e.dart;
	const Dart d2 = m.phi2(d);

	if (m.unsew_faces_topo(d))
	{
		if (m.is_embedded<Vertex>())
		{
			Dart d21 = m.phi1(d2);
			if (m.same_orbit(Vertex(d), Vertex(d21)))
				m.copy_embedding<Vertex>(m.phi2(d2), d21);
			else
				m.new_orbit_embedding(Vertex(d21));

			Dart d1 = m.phi1(d);
			if (m.same_orbit(Vertex(d2), Vertex(d1)))
				m.copy_embedding<Vertex>(m.phi2(d), d1);
			else
				m.new_orbit_embedding(Vertex(d1));
		}

		if (m.is_embedded<Edge>())
			m.new_orbit_embedding(Edge(d2));

		if (m.is_embedded<Volume>())
		{
			if (m.same_orbit(Volume(d), Volume(d2)))
			{
				m.copy_embedding<Volume>(m.phi2(d2), d);
				m.copy_embedding<Volume>(m.phi2(d), d);
			}
			else
				m.new_orbit_embedding(Volume(d2));
		}
	}
}

inline void sew_faces(CMap2& m, Edge e1, Edge e2)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	Dart e1bd = m.boundary_dart(e1);
	Dart e2bd = m.boundary_dart(e2);

	// e1 & e2 should be incident to boundary
	if (e1bd.is_nil() || e2bd.is_nil())
		return;

	Dart e1d = m.phi2(e1bd);
	Dart e2d = m.phi2(e2bd);

	bool v1_merged = m.phi1(e1bd) != e2bd;
	bool v2_merged = m.phi1(e2bd) != e1bd;

	bool volume_merged = false;
	if (m.is_embedded<Volume>())
	{
		if (!m.same_orbit(Volume(e1d), Volume(e2d)))
			volume_merged = true;
	}

	m.sew_faces_topo(e1d, e2d);

	if (m.is_embedded<Vertex>())
	{
		if (v1_merged)
			m.set_orbit_embedding<Vertex>(Vertex(e1d), m.embedding(Vertex(e1d)));
		if (v2_merged)
			m.set_orbit_embedding<Vertex>(Vertex(e2d), m.embedding(Vertex(e2d)));
	}

	if (m.is_embedded<Edge>())
		m.copy_embedding<Edge>(e2d, e1d);

	if (m.is_embedded<Volume>())
	{
		if (volume_merged)
			m.set_orbit_embedding<Volume>(Volume(e1d), m.embedding(Volume(e1d)));
	}
}

void merge_volumes(CMap2& m, Dart d, Dart e)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	std::vector<Dart>* darts = dart_buffers()->buffer();
	std::vector<uint32>* v_emb = uint_buffers()->buffer();
	std::vector<uint32>* e_emb = uint_buffers()->buffer();

	Dart f_it = d ;
	do
	{
		darts->push_back(m.phi2(f_it)) ;

		if (m.is_embedded<Vertex>())
			v_emb->push_back(m.embedding(Vertex(m.phi2(f_it))));

		if (m.is_embedded<Edge>())
			e_emb->push_back(m.embedding(Edge(f_it)));

		f_it = m.phi1(f_it);
	} while(f_it != d);

	if (m.is_embedded<Volume>())
		m.set_orbit_embedding<Volume>(Volume(e), m.embedding(Volume(d)));

	m.merge_volumes_topo(d, e);

	for (uint32 i = 0 ; i < darts->size() ; ++i)
	{
		if (m.is_embedded<Vertex>())
			m.set_orbit_embedding<Vertex>(Vertex((*darts)[i]), (*v_emb)[i]);

		if (m.is_embedded<Edge>())
			m.set_orbit_embedding<Edge>(Edge((*darts)[i]), (*e_emb)[i]);
	}

	dart_buffers()->release_buffer(darts);
	uint_buffers()->release_buffer(v_emb);
	uint_buffers()->release_buffer(e_emb);
}

Face close_hole(CMap2& m, Dart d)
{
	const Face f(m.close_hole_topo(d));

	if (m.is_embedded<Vertex>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart it)
		{
			m.copy_embedding<Vertex>(it, m.phi1(m.phi2(it)));
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.foreach_dart_of_orbit(f, [&m] (Dart it)
		{
			m.copy_embedding<Edge>(it, m.phi2(it));
		});
	}

	if (m.is_embedded<Volume>())
		m.set_orbit_embedding<Volume>(f, m.embedding(Volume(d)));

	return f;
}


uint32 close_map(CMap2& m)
{
	uint32 nb_holes = 0u;

	std::vector<Dart>* fix_point_darts = dart_buffers()->buffer();
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
			m.boundary_mark(f);
			++nb_holes;
		}
	}

	dart_buffers()->release_buffer(fix_point_darts);

	return nb_holes;
}

void reverse_orientation(CMap2& m)
{
	auto& topo = m.topology_container();

	if (m.is_embedded<Vertex::ORBIT>())
	{
		auto& emb0 = m.embeddings_array<Vertex::ORBIT>();
		auto* new_emb0 = topo.add_chunk_array<uint32>("new_EMB_0");

		m.foreach_dart([&m,&emb0, &new_emb0](Dart d)
		{
			(*new_emb0)[d.index] = emb0[m.phi1(d).index];
		});

		emb0.swap_data(new_emb0);
		topo.remove_chunk_array(new_emb0);
	}

	topo.swap_chunk_arrays(topo.get_chunk_array("phi1"),topo.get_chunk_array("phi_1"));
}




} // namespace cgogn


