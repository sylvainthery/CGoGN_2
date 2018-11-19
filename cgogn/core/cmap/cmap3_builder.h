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

#ifndef CGOGN_CORE_CMAP_CMAP3_BUILDER_H_
#define CGOGN_CORE_CMAP_CMAP3_BUILDER_H_

#include <cgogn/core/cmap/map_base.h>
#include <cgogn/core/cmap/cmap3.h>
namespace cgogn
{
using CDart   = CMap3::CDart;
using Vertex2 = CMap3::Vertex2;
using Vertex  = CMap3::Vertex;
using Edge2   = CMap3::Edge2;
using Edge    = CMap3::Edge;
using Face2   = CMap3::Face2;
using Face    = CMap3::Face;
using Volume  = CMap3::Volume;
using Boundary = CMap3::Boundary;
using ConnectedComponent = CMap3::ConnectedComponent;

Vertex cut_edge(CMap3& m, Edge e)
{
	const Dart v = m.cut_edge_topo(e.dart);

	if (m.is_embedded<CDart>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			Dart nv1 = m.phi1(d);
			Dart nv2 = m.phi2(d);
			if (!m.is_boundary_cell(CDart(nv1))) m.new_orbit_embedding(CDart(nv1));
			if (!m.is_boundary_cell(CDart(nv2))) m.new_orbit_embedding(CDart(nv2));
		});
	}

	if (m.is_embedded<Vertex2>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			if (!m.is_boundary_cell(Vertex2(m.phi1(d))))
				m.new_orbit_embedding(Vertex2(m.phi1(d)));
		});
	}

	if (m.is_embedded<Vertex>())
		m.new_orbit_embedding(Vertex(v));

	if (m.is_embedded<Edge2>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			if (!m.is_boundary_cell(Edge2(d)))
			{
				m.copy_embedding<Edge2>(m.phi2(d), d);
				m.new_orbit_embedding(Edge2(m.phi1(d)));
			}
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			m.copy_embedding<Edge>(m.phi2(d), d);
		});
		m.new_orbit_embedding(Edge(v));
	}

	if (m.is_embedded<Face2>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			if (!m.is_boundary_cell(Face2(d)))
			{
				m.copy_embedding<Face2>(m.phi1(d), d);
				m.copy_embedding<Face2>(m.phi2(d), m.phi2(m.phi1(d)));
			}
		});
	}

	if (m.is_embedded<Face>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			m.copy_embedding<Face>(m.phi1(d), d);
			m.copy_embedding<Face>(m.phi3(d), d);
		});
	}

	if (m.is_embedded<Volume>())
	{
		m.foreach_dart_of_PHI23(e.dart, [&m] (Dart d)
		{
			if (!m.is_boundary_cell(Volume(d)))
			{
				m.copy_embedding<Volume>(m.phi1(d), d);
				m.copy_embedding<Volume>(m.phi2(d), d);
			}
		});
	}

	return Vertex(v);
}



 bool flip_edge(CMap3& m, Edge e)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	if (!m.flip_edge_topo(e.dart))
		return false;

	const Dart e2 = m.phi2(e.dart);
	const Dart e3 = m.phi3(e.dart);
	const Dart e32 = m.phi2(e3);

	if (m.is_embedded(Vertex2::ORBIT))
	{
		if (!m.is_boundary_cell(Vertex2(e.dart)))
		{
			m.copy_embedding<Vertex2>(e.dart, m.phi1(e2));
			m.copy_embedding<Vertex2>(e2, m.phi1(e.dart));
		}
		if (!m.is_boundary_cell(Vertex2(e3)))
		{
			m.copy_embedding<Vertex2>(e3, m.phi1(e32));
			m.copy_embedding<Vertex2>(e32, m.phi1(e3));
		}
	}

	if (m.is_embedded(Vertex::ORBIT))
	{
		m.copy_embedding<Vertex>(e.dart, m.phi1(e2));
		m.copy_embedding<Vertex>(e2, m.phi1(e.dart));
		m.copy_embedding<Vertex>(e3, m.phi1(e32));
		m.copy_embedding<Vertex>(e32, m.phi1(e3));
	}

	if (m.is_embedded(Face2::ORBIT))
	{
		if (!m.is_boundary_cell(Face2(e.dart)))
		{
			m.copy_embedding<Face2>(m.phi_1(e.dart), e.dart);
			m.copy_embedding<Face2>(m.phi_1(e2), e2);
		}
		if (!m.is_boundary_cell(Face2(e3)))
		{
			m.copy_embedding<Face2>(m.phi1(e3), e3);
			m.copy_embedding<Face2>(m.phi1(e32), e32);
		}
	}

	if (m.is_embedded(Face::ORBIT))
	{
		m.copy_embedding<Face>(m.phi_1(e.dart), e.dart);
		m.copy_embedding<Face>(m.phi_1(e2), e2);
		m.copy_embedding<Face>(m.phi1(e3), e3);
		m.copy_embedding<Face>(m.phi1(e32), e32);
	}

	return true;
}

/**
 * @brief Flip an Edge (rotation in phi_1 order)
 * @param e : the edge to flip
 * The edge has to be incident to exactly 2 faces.
 */
 bool flip_back_edge(CMap3& m, Edge e)
{
	CGOGN_CHECK_CONCRETE_TYPE;

	if (!m.flip_back_edge_topo(e.dart))
		return false;

	const Dart e2 = m.phi2(e.dart);
	const Dart e3 = m.phi3(e.dart);
	const Dart e32 = m.phi2(e3);

	if (m.is_embedded(Vertex2::ORBIT))
	{
		if (!m.is_boundary_cell(Vertex2(e.dart)))
		{
			m.copy_embedding<Vertex2>(e.dart, m.phi1(e2));
			m.copy_embedding<Vertex2>(e2, m.phi1(e.dart));
		}
		if (!m.is_boundary_cell(Vertex2(e3)))
		{
			m.copy_embedding<Vertex2>(e3, m.phi1(e32));
			m.copy_embedding<Vertex2>(e32, m.phi1(e3));
		}
	}

	if (m.is_embedded(Vertex::ORBIT))
	{
		m.copy_embedding<Vertex>(e.dart, m.phi1(e2));
		m.copy_embedding<Vertex>(e2, m.phi1(e.dart));
		m.copy_embedding<Vertex>(e3, m.phi1(e32));
		m.copy_embedding<Vertex>(e32, m.phi1(e3));
	}

	if (m.is_embedded(Face2::ORBIT))
	{
		if (!m.is_boundary_cell(Face2(e.dart)))
		{
			m.copy_embedding<Face2>(m.phi1(e.dart), e.dart);
			m.copy_embedding<Face2>(m.phi1(e2), e2);
		}
		if (!m.is_boundary_cell(Face2(e3)))
		{
			m.copy_embedding<Face2>(m.phi_1(e3), e3);
			m.copy_embedding<Face2>(m.phi_1(e32), e32);
		}
	}

	if (m.is_embedded(Face::ORBIT))
	{
		m.copy_embedding<Face>(m.phi1(e.dart), e.dart);
		m.copy_embedding<Face>(m.phi1(e2), e2);
		m.copy_embedding<Face>(m.phi_1(e3), e3);
		m.copy_embedding<Face>(m.phi_1(e32), e32);
	}

	return true;
}


 Dart split_vertex(CMap3& m, std::vector<Dart>& vd)
{
	const Dart d1 = vd.front();
	const Dart d2 = m.phi1(m.phi2(d1));
	const Dart res = m.split_vertex_topo(vd);

	if (m.is_embedded<CDart>())
	{
		// TODO ...
		cgogn_log_debug("CMap3::split_vertex") << "the CDart embeddings are not updated.";
	}

	if (m.is_embedded<Vertex2>())
	{
		// TODO ...
		cgogn_log_debug("CMap3::split_vertex") << "the Vertex2 embeddings are not updated.";
	}

	if (m.is_embedded<Vertex>())
	{
		m.new_orbit_embedding(Vertex(d2));
		m.set_orbit_embedding<Vertex>(Vertex(d1), m.embedding(Vertex(d1)));
	}

	if (m.is_embedded<Edge>())
		m.new_orbit_embedding(Edge(res));

	if (m.is_embedded<Face2>())
	{
		// TODO ...
		cgogn_log_debug("CMap3::split_vertex") << "the Face2 embeddings are not updated.";
	}

	if (m.is_embedded<Face>())
	{
		// TODO ...
		cgogn_log_debug("CMap3::split_vertex") << "the Face embeddings are not updated.";
	}

	if (m.is_embedded<Volume>())
	{
		for (auto dit1 : vd)
			m.set_orbit_embedding<Volume>(Volume(dit1), m.embedding(Volume(dit1)));
	}

	return res;
}


/**
 * \brief Cut a face by inserting an edge between the vertices of d and e
 * \param d : a dart of the first vertex
 * \param e : a dart of the second vertex
 * \return The inserted edge
 * The darts d and e should belong to the same Face2 and be distinct from each other.
 * An edge is inserted between the two given vertices.
 * The returned edge is represented by the dart of the inserted edge that belongs to the Face2 of d.
 * If the map has Dart, Vertex2, Vertex, Edge2, Edge, Face2, Face or Volume attributes,
 * the inserted cells are automatically embedded on new attribute elements.
 * More precisely :
 *  - two Edge2 attribute are created, if needed, for the inserted Edge2.
 *  - an Edge attribute is created, if needed, for the inserted edge.
 *  - two Face2 attributes are created, if needed, for the subdivided Face2 of e and phi3(e).
 *  - a Face attribute is created, if needed, for the subdivided face that e belongs to.
 *  - the Face attribute of the subdivided face that d belongs to is kept unchanged.
 */
 Edge cut_face(CMap3& m, Dart d, Dart e)
{
	Dart nd = m.cut_face_topo(d, e);
	Dart ne = m.phi_1(e);
	Dart nd3 = m.phi3(nd);
	Dart ne3 = m.phi3(ne);

	if (m.is_embedded<CDart>())
	{
		if (!m.is_boundary_cell(CDart(nd))) m.new_orbit_embedding(CDart(nd));
		if (!m.is_boundary_cell(CDart(ne))) m.new_orbit_embedding(CDart(ne));
		if (!m.is_boundary_cell(CDart(nd3))) m.new_orbit_embedding(CDart(nd3));
		if (!m.is_boundary_cell(CDart(ne3))) m.new_orbit_embedding(CDart(ne3));
	}

	if (m.is_embedded<Vertex2>())
	{
		if (!m.is_boundary_cell(Vertex2(nd)))
		{
			m.copy_embedding<Vertex2>(nd, e);
			m.copy_embedding<Vertex2>(ne, d);
		}
		if (!m.is_boundary_cell(Vertex2(nd3)))
		{
			m.copy_embedding<Vertex2>(nd3, m.phi1(ne3));
			m.copy_embedding<Vertex2>(ne3, m.phi1(nd3));
		}
	}

	if (m.is_embedded<Vertex>())
	{
		m.copy_embedding<Vertex>(nd, e);
		m.copy_embedding<Vertex>(ne3, e);
		m.copy_embedding<Vertex>(ne, d);
		m.copy_embedding<Vertex>(nd3, d);
	}

	if (m.is_embedded<Edge2>())
	{
		if (!m.is_boundary_cell(Edge2(nd)))
			m.new_orbit_embedding(Edge2(nd));
		if (!m.is_boundary_cell(Edge2(nd3)))
			m.new_orbit_embedding(Edge2(nd3));
	}

	if (m.is_embedded<Edge>())
		m.new_orbit_embedding(Edge(nd));

	if (m.is_embedded<Face2>())
	{
		if (!m.is_boundary_cell(Face2(nd)))
		{
			m.copy_embedding<Face2>(nd, d);
			m.new_orbit_embedding(Face2(ne));
		}
		if (!m.is_boundary_cell(Face2(nd3)))
		{
			m.copy_embedding<Face2>(nd3, m.phi3(d));
			m.new_orbit_embedding(Face2(ne3));
		}
	}

	if (m.is_embedded<Face>())
	{
		m.copy_embedding<Face>(nd, d);
		m.copy_embedding<Face>(nd3, d);
		m.new_orbit_embedding(Face(ne));
	}

	if (m.is_embedded<Volume>())
	{
		if (!m.is_boundary_cell(Volume(d)))
		{
			m.copy_embedding<Volume>(nd, d);
			m.copy_embedding<Volume>(ne, d);
		}
		Dart d3 = m.phi3(d);
		if (!m.is_boundary_cell(Volume(d3)))
		{
			m.copy_embedding<Volume>(nd3, d3);
			m.copy_embedding<Volume>(ne3, d3);
		}
	}

	return Edge(nd);
}


/**
 * \brief Merge the two faces incident to the given edge
 * \param e : the edge
 * The edge has to be incident to exactly 2 faces.
 */
bool merge_incident_faces(CMap3& m, Edge e)
{

	const Dart f = m.phi1(e.dart);

	if (m.merge_incident_faces_of_edge_topo(e.dart))
	{
		if (m.is_embedded<Face2>())
		{
			if (!m.is_boundary_cell(Face2(f)))
				m.set_orbit_embedding<Face2>(Face2(f), m.embedding(Face2(f)));

			const Dart f3 = m.phi3(f);
			if (!m.is_boundary_cell(Face2(f3)))
				m.set_orbit_embedding<Face2>(Face2(f3), m.embedding(Face2(f3)));
		}

		if (m.is_embedded<Face>())
			m.set_orbit_embedding<Face>(Face(f), m.embedding(Face(f)));

		return true;
	}

	return false;
}


/**
 * \brief Merge the volumes incident to the edge e by removing the edge
 * \param e : the edge
 * \return true if the volumes have been merged, false otherwise
 * If the edge is incident to the boundary, nothing is done.
 */
Dart merge_incident_volumes(CMap3& m, Edge e)
{
	const Dart res_topo_del = merge_incident_volumes_of_edge_topo(e.dart);
	if (res_topo_del.is_nil())
		return res_topo_del;

	if (m.is_embedded<Vertex2>())
	{
		// TODO ...
		cgogn_log_debug("CMap3::merge_incident_volumes(Edge)") << "the Vertex2 embeddings are not updated.";
	}

	if (m.is_embedded<Edge2>())
	{
		// TODO ...
		cgogn_log_debug("CMap3::merge_incident_volumes(Edge)") << "the Edge2 embeddings are not updated.";
	}

	if (m.is_embedded<Volume>())
		m.set_orbit_embedding<Volume>(Volume(res_topo_del), m.embedding(Volume(res_topo_del)));

	return res_topo_del;
}


/**
 * \brief Merge the volumes incident to the face f by removing the face
 * \param f : the face
 * \return true if the volumes have been merged, false otherwise
 * If the face is incident to the boundary, nothing is done.
 */
bool merge_incident_volumes(CMap3& m, Face f)
{
	std::vector<Dart>* face_path = cgogn::dart_buffers()->buffer();
	m.foreach_dart_of_orbit(f, [face_path, this] (Dart d)
	{
		face_path->push_back(m.phi2(d));
	});

	if (merge_incident_volumes_of_face_topo(f.dart))
	{
		if (m.is_embedded<Vertex2>())
		{
			for (Dart d : *face_path)
				m.set_orbit_embedding<Vertex2>(Vertex2(d), m.embedding(Vertex2(d)));
		}

		if (m.is_embedded<Edge2>())
		{
			for (Dart d : *face_path)
				m.copy_embedding<Edge2>(m.phi2(d), d);
		}

		if (m.is_embedded<Volume>())
		{
			Dart d = face_path->front();
			m.set_orbit_embedding<Volume>(Volume(d), m.embedding(Volume(d)));
		}

		return true;
	}

	return false;
}

/**
 * @brief Cut a single volume following a simple closed oriented path
 * @param path a vector of darts representing the path
 * @return the inserted face
 */
Face cut_volume(CMap3& m, const std::vector<Dart>& path)
{
	cgogn_message_assert(!m.is_boundary_cell(Volume(path[0])), "cut_volume: should not cut a boundary volume");

	Dart nf = cut_volume_topo(path);

	if (m.is_embedded<CDart>())
	{
		m.foreach_dart_of_orbit(Face2(nf), [&m] (Dart d)
		{
			m.new_orbit_embedding(CDart(d));
			m.new_orbit_embedding(CDart(m.phi3(d)));
		});
	}

	if (m.is_embedded<Vertex2>())
	{
		m.foreach_dart_of_orbit(Face2(nf), [&m] (Dart d)
		{
			m.copy_embedding<Vertex2>(d, m.phi1(m.phi2(d)));
			m.new_orbit_embedding(Vertex2(m.phi3(d)));
		});
	}

	if (m.is_embedded<Vertex>())
	{
		m.foreach_dart_of_orbit(Face2(nf), [&m] (Dart d)
		{
			m.copy_embedding<Vertex>(d, m.phi1(m.phi2(d)));
			Dart d3 = m.phi3(d);
			m.copy_embedding<Vertex>(d3, m.phi1(m.phi2(d3)));
		});
	}

	if (m.is_embedded<Edge2>())
	{
		m.foreach_dart_of_orbit(Face2(nf), [&m] (Dart d)
		{
			m.copy_embedding<Edge2>(d, m.phi2(d));
			m.new_orbit_embedding(Edge2(m.phi3(d)));
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.foreach_dart_of_orbit(Face2(nf), [&m] (Dart d)
		{
			m.copy_embedding<Edge>(d, m.phi2(d));
			m.copy_embedding<Edge>(m.phi3(d), m.phi2(d));
		});
	}

	if (m.is_embedded<Face2>())
	{
		m.new_orbit_embedding(Face2(nf));
		m.new_orbit_embedding(Face2(m.phi3(nf)));
	}

	if (m.is_embedded<Face>())
		m.new_orbit_embedding(Face(nf));

	if (m.is_embedded<Volume>())
	{
		m.foreach_dart_of_orbit(Face2(nf), [&m] (Dart d)
		{
			m.copy_embedding<Volume>(d, m.phi2(d));
		});
		m.new_orbit_embedding(Volume(m.phi3(nf)));
	}

	return Face(nf);
}


/**
 * @brief Sew the volumes incident to the faces fa and fb
 * @param fa, fb : the faces
 * The given faces must have the same codegree and be incident to the boundary
 */
void sew_volumes(CMap3& m, Face fa, Face fb)
{
	if (!sew_volumes_topo(fa.dart, fb.dart))
		return;

	if (m.is_embedded<Vertex>())
	{
		Dart dit = fa.dart;
		do
		{
			m.set_orbit_embedding<Vertex>(Vertex(dit), m.embedding(Vertex(dit)));
			dit = m.phi1(dit);
		} while (dit != fa.dart);
	}

	if (m.is_embedded<Edge>())
	{
		Dart dit = fa.dart;
		do
		{
			m.set_orbit_embedding<Edge>(Edge(dit), m.embedding(Edge(dit)));
			dit = m.phi1(dit);
		} while (dit != fa.dart);
	}

	if (m.is_embedded<Face>())
		m.set_orbit_embedding<Face>(fb, m.embedding(fa));
}



/**
 * @brief Unsew the volumes incident to the face f
 * @param f : the face
 * @return true if the volumes have been unsewn, false otherwise
 * The two volumes are detached, a 2-faced boundary volume is inserted.
 * For each of the edges of the face, if it is already incident to a boundary
 * volume, the new boundary volume is merged with the existing boundary, resulting in a split
 * of the edge into two edges (vertices can be split in the same process).
 */
void unsew_volumes(CMap3& m, Face f)
{
	Dart dd = m.phi3(m.phi_1(f.dart));
	Dart dit = f.dart;

	if (!m.unsew_volumes_topo(f.dart))
		return;

	do
	{
		if (m.is_embedded<Vertex>() && !m.same_orbit(Vertex(dit), Vertex(dd)))
			m.new_orbit_embedding(Vertex(dd));

		dd = m.phi_1(dd);

		if (m.is_embedded<Edge>() && !m.same_orbit(Edge(dit), Edge(dd)))
			m.new_orbit_embedding(Edge(dd));

		dit = m.phi1(dit);
	} while (dit != f.dart);

	if (m.is_embedded<Face>())
		m.new_orbit_embedding(Face(dd));
}


void delete_volume(CMap3& m, Volume w)
{;
	m.delete_volume_topo(w);
}


 Volume close_hole(CMap3& m, Dart d)
{
	const Volume v(close_hole_topo(d));

	if (m.is_embedded<Vertex>())
	{
		m.foreach_dart_of_orbit(v, [&m] (Dart it)
		{
			m.copy_embedding<Vertex>(it, m.phi1(m.phi3(it)));
		});
	}

	if (m.is_embedded<Edge>())
	{
		m.foreach_dart_of_orbit(v, [&m] (Dart it)
		{
			m.copy_embedding<Edge>(it, m.phi3(it));
		});
	}

	if (m.is_embedded<Face>())
	{
		m.foreach_dart_of_orbit(v, [&m] (Dart it)
		{
			m.copy_embedding<Face>(it, m.phi3(it));
		});
	}

	return v;
}

/**
 * @brief close_map closes the map removing topological holes (only for import/creation)
 * Add volumes to the map that close every existing hole.
 * @return the number of closed holes
 */
 uint32 close_map(CMap3& m)
{
	uint32 nb_holes = 0;

	// Search the map for topological holes (fix points of phi3)
	std::vector<Dart>* fix_point_darts = dart_buffers()->buffer();
	m.foreach_dart([&] (Dart d)
	{
		if (m.phi3(d) == d)
			fix_point_darts->push_back(d);
	});

	for (Dart d : (*fix_point_darts))
	{
		if (m.phi3(d) == d)
		{
			Volume v = close_hole(d);
			m.boundary_mark(v);
			++nb_holes;
		}
	}

	dart_buffers()->release_buffer(fix_point_darts);

	return nb_holes;
}


} // namespace cgogn

#endif // CGOGN_CORE_CMAP_CMAP3_BUILDER_H_
