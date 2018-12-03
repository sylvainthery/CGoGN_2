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

#ifndef CGOGN_CORE_CMAP_CMAP1_TOPO_H_
#define CGOGN_CORE_CMAP_CMAP1_TOPO_H_

#include <cgogn/core/cmap/cmap1.h>

namespace cgogn
{
namespace topo
{

	/*******************************************************************************
	 * High-level embedded and topological operations
	 *******************************************************************************/

/**
 * \brief Split a vertex.
 * \param d : a dart of the vertex
 * \return A dart of inserted vertex
 * A new vertex is inserted after v in the PHI1 orbit.
 */
inline Dart split_vertex_topo(CMap1& m, Dart d)
{
	Dart e = m.add_topology_element();	// Create a new dart e
	m.phi1_sew(d, e);							// Insert e between d and phi1(d)
	return e;
}

/*!
 * \brief Add a face in the map.
 * \param size : the number of darts in the built face
 * \return A dart of the built face
 */
inline Dart add_face_topo(CMap1& m, std::size_t size)
{
	cgogn_message_assert(size > 0u, "Cannot create an empty face");

	if (size == 0)
		cgogn_log_warning("add_face_topo") << "Attempt to create an empty face results in a single dart.";

	Dart d = m.add_topology_element();
	for (std::size_t i = 1u; i < size; ++i)
		split_vertex_topo(m,d);
	return d;
}


/*!
 * \brief Remove a face from the map.
 * \param d : a dart of the face to remove
 */
inline void remove_face_topo(CMap1& m, Dart d)
{
	Dart it = m.phi1(d);
	while(it != d)
	{
		Dart next = m.phi1(it);
		m.remove_topology_element(it);
		it = next;
	}
	m.remove_topology_element(d);
}



/**
 * \brief Remove a vertex from its face and delete it.
 * @param d : a dart of the vertex
 * The vertex that preceeds the vertex of d in the face is linked
 * to the successor of the vertex of d.
 */
inline void remove_vertex_topo(CMap1& m, Dart d)
{
	Dart e = m.phi_1(d);
	if (e != d) m.phi1_unsew(e);
	m.remove_topology_element(d);
}


inline void reverse_face_topo(CMap1& m, Dart d)
{
	Dart e = m.phi1(d);			// Dart e is the first edge of the new face

	if (e == d) return;			// Only one edge: nothing to do
	if (m.phi1(e) == d) return;	// Only two edges: nothing to do

	m.phi1_unsew(d);				// Detach e from the face of d

	Dart dNext = m.phi1(d);
	while (dNext != d)			// While the face of d contains more than two edges
	{
		m.phi1_unsew(d);			// Unsew the edge after d
		m.phi1_sew(e, dNext);		// Sew it after e (thus in reverse order)
		dNext = m.phi1(d);
	}
	m.phi1_sew(e, d);				// Sew the last edge
}


/*!
 * \brief Close the topological hole that contains Dart d (a fixed point of phi1 relation)
 * \param d a dart incident to the hole
 * \return a dart of the edge that closes the hole
 * This method is used to close a CMap1 that has been built through the 1-sewing of edges
 * An edge is inserted on the boundary that begins at dart d
 */
inline Dart close_hole_topo(CMap1& m, Dart d)
{
	cgogn_message_assert(m.phi1(d) == d || m.phi_1(d) == d, "CMap1: close hole called on a dart that is not a phi1 fix point");

	Dart first = m.add_topology_element();	// dart that will fill the hole

	if (m.phi_1(d) == d)
	{
		m.phi1_sew(first, d);							// 1-sew the new dart to the hole

		Dart d_next = d;
		Dart d_phi1 = d;
		do
		{
			d_next = d_phi1;
			d_phi1 = m.phi1(d_next);
		} while(d_next != d_phi1);

		m.phi1_sew(d_phi1, first);
	}

	return first;
}




	/*******************************************************************************
	 * Connectivity information
	 *******************************************************************************/

inline uint32 degree(CMap1&, CMap1::Vertex)
{
	return 1;
}

inline uint32 codegree(CMap1& m, CMap1::Face f)
{
	m.nb_darts_of_orbit(f);
}

inline bool has_codegree(CMap1& m, CMap1::Face f, uint32 codegree)
{
	if (codegree < 1u) return false;
	Dart it = f.dart ;
	for (uint32 i = 1u; i < codegree; ++i)
	{
		it = m.phi1(it) ;
		if (it == f.dart)
			return false;
	}
	it = m.phi1(it) ;
	return (it == f.dart);
}

	/*******************************************************************************
	* Orbits traversal                                                             *
	*******************************************************************************/

template <Orbit ORBIT, typename FUNC>
inline void foreach_dart_of_orbit(CMap1& m, Cell<ORBIT> c, const FUNC& f)
{
	static_assert(is_func_parameter_same<FUNC, Dart>::value, "Wrong function parameter type");
	static_assert(ORBIT == Orbit::DART || ORBIT == Orbit::PHI1, "Orbit not supported in a CMap1");

	switch (ORBIT)
	{
		case Orbit::DART: f(c.dart); break;
		case Orbit::PHI1: m.foreach_dart_of_PHI1(c.dart, f); break;
		case Orbit::PHI2:
		case Orbit::PHI21:
		case Orbit::PHI1_PHI2:
		case Orbit::PHI1_PHI3:
		case Orbit::PHI2_PHI3:
		case Orbit::PHI21_PHI31:
		case Orbit::PHI1_PHI2_PHI3:
		default: cgogn_assert_not_reached("Orbit not supported in a CMap1"); break;
	}
}

template <Orbit ORBIT>
inline bool is_boundary_cell(CMap1& m, Cell<ORBIT> c) const
{
	switch (ORBIT)
	{
		case Orbit::DART: return m.is_boundary(c.dart); break;
		case Orbit::PHI1: return false; break;
		case Orbit::PHI2:
		case Orbit::PHI21:
		case Orbit::PHI1_PHI2:
		case Orbit::PHI1_PHI3:
		case Orbit::PHI2_PHI3:
		case Orbit::PHI21_PHI31:
		case Orbit::PHI1_PHI2_PHI3:
		default: cgogn_assert_not_reached_false("Orbit not supported in a CMap1"); break;
	}
}


} // namespace topo
} // namespace cgogn

#endif // CGOGN_CORE_CMAP_CMAP1_TOPO_H_
