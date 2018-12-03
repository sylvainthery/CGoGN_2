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

#ifndef CGOGN_CORE_CMAP_CMAP2_H_
#define CGOGN_CORE_CMAP_CMAP2_H_

#include <cgogn/core/cmap/cmap1.h>

namespace cgogn
{

namespace topo
{

	/*******************************************************************************
	 * High-level embedded and topological operations
	 *******************************************************************************/

	/**
	 * \brief Add a face in the map.
	 * \param size : the number of darts in the built face
	 * \return A dart of the built face.
	 * Two 1-face are built. The first one is the returned face,
	 * the second is a boundary face that closes the map.
	 */
	Dart add_face_topo(std::size_t size)
	{
		Dart d = Inherit::add_face_topo(size);
		Dart e = Inherit::add_face_topo(size);

		this->foreach_dart_of_PHI1(d, [&] (Dart it)
		{
			this->set_boundary(e, true);
			phi2_sew(it, e);
			e = this->phi_1(e);
		});

		return d;
	}

	/**
	 * \brief Add a face whose darts are in fixed point for phi2 relation
	 * \param size : the number of darts in the built face
	 * \return A dart of the built face.
	 */
	Dart add_face_topo_fp(std::size_t size)
	{
		return Inherit::add_face_topo(size);
	}

	void remove_face_topo_fp(Dart d)
	{
		Inherit::remove_face_topo(d);
	}




	/**
	 * \brief Add a pyramid whose base has n sides.
	 * \param size : the number of darts in the base face
	 * \return A dart of the base face
	 * The base is a face with n vertices and edges.
	 * Each edge is adjacent to a triangular face.
	 * These triangles are pairwise sewn to build the top of the pyramid.
	 */
	inline Dart add_pyramid_topo(std::size_t size)
	{
		cgogn_message_assert(size > 0u, "The pyramid cannot be empty");

		Dart first = this->Inherit::add_face_topo(3u);	// First triangle
		Dart current = first;

		for (uint32 i = 1u; i < size; ++i)				// Next triangles
		{
			Dart next = this->Inherit::add_face_topo(3u);
			this->phi2_sew(this->phi_1(current), this->phi1(next));
			current = next;
		}
														// End the umbrella
		this->phi2_sew(this->phi_1(current), this->phi1(first));

		return this->close_hole_topo(first);			// Add the base face
	}



	/**
	 * \brief Add a prism with n sides.
	 * \param size : the number of sides of the prism
	 * \return A dart of the base face
	 * The base and the top are faces with n vertices and edges.
	 * A set of n pairwise linked quads are built.
	 * These quads are sewn to the base and top faces.
	 */
	Dart add_prism_topo(std::size_t size)
	{
		cgogn_message_assert(size > 0u, "The prism cannot be empty");

		Dart first = this->Inherit::add_face_topo(4u);			// First quad
		Dart current = first;

		for (uint32 i = 1u; i < size; ++i)						// Next quads
		{
			Dart next = this->Inherit::add_face_topo(4u);
			phi2_sew(this->phi_1(current), this->phi1(next));
			current = next;
		}

		phi2_sew(this->phi_1(current), this->phi1(first)); // Close the quad strip

		close_hole_topo(this->phi1(this->phi1(first)));	// Add the top face

		return close_hole_topo(first);					// Add the base face
	}


	/*!
	 * \brief Remove a volume from the map.
	 * \param d : a dart of the volume to remove
	 */
	inline void remove_volume_topo(Dart d)
	{
		std::vector<Dart>* darts = dart_buffers()->buffer();

		foreach_dart_of_orbit(Volume(d),[&] (Dart e)
		{
			darts->push_back(e);
		});

		for (Dart e: *darts)
			this->remove_topology_element(e);
	}

	/**
	 * \brief Cut an edge.
	 * \param d : A dart that represents the edge to cut
	 * \return A dart of the inserted vertex
	 * The edge of d is cut by inserting a new vertex.
	 * The returned dart is the dart of the inserted vertex that belongs to the face of d.
	 */
	inline Dart cut_edge_topo(Dart d)
	{
		Dart e = phi2(d);						// Get the adjacent 1D-edge

		phi2_unsew(d);							// Separate the two 1D-edges of the edge

		Dart nd = this->Inherit::split_vertex_topo(d);
		Dart ne = this->Inherit::split_vertex_topo(e);	// Cut the two adjacent 1D-edges

		phi2_sew(d, ne);						// Sew the new 1D-edges
		phi2_sew(e, nd);						// To build the new 2D-edges

		this->set_boundary(nd, this->is_boundary(d));
		this->set_boundary(ne, this->is_boundary(e));

		return nd;
	}

	/**
	 * @brief Flip an edge
	 * @param d : a dart of the edge to flip
	 * @return true if the edge has been flipped, false otherwise
	 * Each end of the edge is detached from its initial vertex
	 * and inserted in the next vertex within its incident face.
	 * An end of the edge that is a vertex of degree 1 is not moved.
	 * If one of the faces have co-degree 1 then nothing is done.
	 */
	inline bool flip_edge_topo(Dart d)
	{
		if (!this->is_incident_to_boundary(Edge(d)) || this->dimension_concrete_map() == 3u)
		{
			Dart e = phi2(d);
			Dart d1 = this->phi1(d);
			Dart d_1 = this->phi_1(d);
			Dart e1 = this->phi1(e);
			Dart e_1 = this->phi_1(e);

			// Cannot flip edge whose incident faces have co-degree 1
			if (d == d1  || e == e1) return false;

			// Both vertices have degree 1 and thus nothing is done // TODO may return true ?
			if (d == e_1 && e == d_1) return false;

			if (d != e_1) this->phi1_sew(d, e_1);	// Detach the edge from its
			if (e != d_1) this->phi1_sew(e, d_1);	// two incident vertices

			if (d != e_1) this->phi1_sew(d, d1);	// Insert the first end in its new vertices
			if (e != d_1) this->phi1_sew(e, e1);	// Insert the second end in its new vertices

			return true;
		}
		return false;
	}

	inline bool flip_back_edge_topo(Dart d)
	{
		if (!this->is_incident_to_boundary(Edge(d)) || this->dimension_concrete_map() == 3u)
		{
			const Dart e = phi2(d);
			const Dart d_1 = this->phi_1(d);
			const Dart e_1= this->phi_1(e);
			this->phi1_sew(d, e_1);				// Detach the two
			this->phi1_sew(e, d_1);				// vertices of the edge
			this->phi1_sew(e, this->phi_1(d_1));	// Insert the edge in its
			this->phi1_sew(d, this->phi_1(e_1));	// new vertices after flip
			return true;
		}
		return false;
	}


	/**
	 * @brief Collapse an edge
	 * @param d : a dart of the edge to collapse
	 * @return a dart of the resulting vertex
	 */
	inline Dart collapse_edge_topo(Dart d)
	{
		Dart d_1 = this->phi_1(d);
		Dart e = phi2(d);
		Dart e_1 = this->phi_1(e);

		Dart res = phi2(d_1);

		this->remove_vertex_topo(d);
		this->remove_vertex_topo(e);

		if (codegree(Face(d_1)) == 2u)
		{
			Dart d1 = this->phi1(d_1);
			Dart d12 = phi2(d1);
			Dart d_12 = phi2(d_1);

			phi2_unsew(d1);
			phi2_unsew(d_1);

			phi2_sew(d12, d_12);
			this->Inherit::remove_face_topo(d1);
		}

		if (codegree(Face(e_1)) == 2u)
		{
			Dart e1 = this->phi1(e_1);
			Dart e12 = phi2(e1);
			Dart e_12 = phi2(e_1);

			phi2_unsew(e1);
			phi2_unsew(e_1);

			phi2_sew(e12, e_12);
			this->Inherit::remove_face_topo(e1);
		}

		return res;
	}

	inline void split_vertex_topo(Dart d, Dart e)
	{
		cgogn_assert(this->same_orbit(Vertex(d), Vertex(e)));

		const Dart d2 = phi2(d);
		const Dart e2 = phi2(e);
		cgogn_assert(d != d2);
		cgogn_assert(e != e2);
		const Dart nd = Inherit::split_vertex_topo(d2);
		const Dart ne = Inherit::split_vertex_topo(e2);
		phi2_sew(nd,ne);
	}


	bool merge_incident_edges_topo(Dart d)
	{
		Dart d2 = phi2(d);
		Dart e = this->phi1(d2);
		Dart e2 = phi2(e);

		if (d != this->phi1(e2)) // the degree of the vertex of d should be 2
			return false;

		phi2_unsew(d);
		phi2_unsew(e);
		this->remove_vertex_topo(d);
		this->remove_vertex_topo(e);
		phi2_sew(d2, e2);
		return true;
	}

	/**
	 * \brief Merge the two faces incident to the edge of d by removing the edge of d
	 * \param d : dart of the edge
	 * \return true if the faces have been merged, false otherwise
	 * The endpoints of the edge of d are detached from their vertex and then the edge is removed.
	 * If the edge of d is incident to the boundary, nothing is done.
	 */
	bool merge_incident_faces_of_edge_topo(Dart d)
	{
		if (this->is_incident_to_boundary(Edge(d)))
			return false;

		Dart d2 = phi2(d);
		this->phi1_sew(this->phi_1(d), d2);
		this->phi1_sew(this->phi_1(d2), d);
		this->Inherit::remove_face_topo(d);

		return true;
	}


	/**
	 * \brief Merge the faces incident to the vertex of d
	 * \param d : dart of the edge
	 * \return true if the faces have been merged, false otherwise
	 * The opposite endpoints of the edges of the vertex are detached from their vertex and then the so constructed face is removed.
	 * If the vertex of d is incident to the boundary, nothing is done.
	 */
	bool merge_incident_faces_of_vertex_topo(Dart d)
	{
		if (this->is_incident_to_boundary(Vertex(d)))
			return false;

		Dart it = d;
		do
		{
			Dart f = this->phi_1(phi2(it));
			this->phi1_sew(it, f);
			it = phi2(this->phi_1(it));
		} while (it != d);
		Inherit::remove_face_topo(d);

		return true;
	}

	/**
	 * \brief Cut the face of d and e by inserting an edge between the vertices of d and e
	 * \param d : first vertex
	 * \param e : second vertex
	 * \return A dart of the inserted edge
	 * Darts d and e should belong to the same face and be distinct from each other.
	 * An edge made of two new darts is inserted between the two given vertices.
	 * The returned dart is the dart of the inserted edge that belongs to the face of d.
	 */
	inline Dart cut_face_topo(Dart d, Dart e)
	{
		cgogn_message_assert(d != e, "cut_face_topo: d and e should be distinct");
		cgogn_message_assert(this->same_cell(Face(d), Face(e)), "cut_face_topo: d and e should belong to the same face");

		Dart dd = this->phi_1(d);
		Dart ee = this->phi_1(e);
		Dart nd = Inherit::split_vertex_topo(dd);	// cut the edge before d (insert a new dart before d)
		Dart ne = Inherit::split_vertex_topo(ee);	// cut the edge before e (insert a new dart before e)
		this->phi1_sew(dd, ee);						// subdivide phi1 cycle at the inserted darts
		phi2_sew(nd, ne);							// build the new 2D-edge from the inserted darts

		this->set_boundary(nd, this->is_boundary(dd));
		this->set_boundary(ne, this->is_boundary(ee));

		return nd;
	}


	/**
	 * @brief Unsew the faces incident to the edge of d
	 * @param d : dart of the edge
	 * @return true if the faces have been unsewn, false otherwise
	 * The two faces are detached, a 2-sided boundary face is inserted.
	 * For each of the two end vertices of the edge, if it is already incident to a boundary
	 * face, the new boundary face is merged with the existing boundary, resulting in a separation
	 * of the vertex into two vertices.
	 */
	inline bool unsew_faces_topo(Dart d)
	{
		if (this->is_incident_to_boundary(Edge(d)))
			return false;

		Dart dd = phi2(d);

		Dart e = Inherit::add_face_topo(2);
		Dart ee = this->phi1(e);
		this->set_boundary(e, true);
		this->set_boundary(ee, true);

		Dart f = this->boundary_dart(Vertex(d));
		Dart ff = this->boundary_dart(Vertex(dd));

		if (!f.is_nil())
			this->phi1_sew(e, this->phi_1(f));

		if (!ff.is_nil())
			this->phi1_sew(ee, this->phi_1(ff));

		phi2_unsew(d);
		phi2_sew(d, e);
		phi2_sew(dd, ee);

		return true;
	}



	inline void sew_faces_topo(Dart d, Dart e)
	{
		Dart d2 = phi2(d);
		Dart e2 = phi2(e);

		phi2_unsew(d);
		phi2_unsew(e);

		if (this->phi1(e2) != d2)
		{
			cgogn_assert(!this->same_orbit(Vertex(e), Vertex(d2)));
			// two different boundary vertices: merge boundary faces
			this->phi1_sew(e2, this->phi_1(d2));
		}
		if (this->phi1(d2) != e2)
		{
			cgogn_assert(!this->same_orbit(Vertex(d), Vertex(e2)));
			// two different boundary vertices: merge boundary faces
			this->phi1_sew(d2, this->phi_1(e2));
		}

		Inherit::remove_face_topo(d2); // remove 2-sided boundary face
		phi2_sew(d, e);
	}

	/**
	 * @brief merge incident volumes along the given faces
	 * @param d dart of the first face
	 * @param e dart of the second face
	 */
	void merge_volumes_topo(Dart d, Dart e)
	{
		//Traversal of both faces to check their sizes
		//and store their edges to efficiently access them further
		cgogn_assert(codegree(Face(d)) == codegree(Face(e)));

		std::vector<Dart>* d_darts = dart_buffers()->buffer();
		std::vector<Dart>* e_darts = dart_buffers()->buffer();

		Dart d_fit = d;
		Dart e_fit = e;
		do
		{
			d_darts->push_back(d_fit);
			d_fit = this->phi1(d_fit);

			e_darts->push_back(e_fit);
			e_fit = this->phi_1(e_fit);
		} while(d_fit != d);

		std::vector<Dart>::iterator d_it, e_it;

		for (d_it = d_darts->begin(), e_it = e_darts->begin();
			 d_it != d_darts->end(); ++d_it, ++e_it)
		{
			// Search the faces adjacent to d_it and e_it
			Dart d2 = phi2(*d_it);
			Dart e2 = phi2(*e_it);
			// Unlink the two adjacent faces from d_it and e_it
			phi2_unsew(d2);
			phi2_unsew(e2);
			// Link the two adjacent faces together
			phi2_sew(d2, e2);
		}

		dart_buffers()->release_buffer(d_darts);
		dart_buffers()->release_buffer(e_darts);

		// Delete the two alone faces
		this->Inherit::remove_face_topo(d);
		this->Inherit::remove_face_topo(e);
	}

	/*!
	 * \brief Close the topological hole that contains Dart d (a fixed point of phi2 relation)
	 * \param d a dart incident to the hole
	 * \return a dart of the face that closes the hole
	 * This method is used to close a CMap2 that has been built through the 2-sewing of 1-faces
	 * A face is inserted on the boundary that begins at dart d
	 */
	inline Dart close_hole_topo(Dart d)
	{
		cgogn_message_assert(phi2(d) == d, "CMap2: close hole called on a dart that is not a phi2 fix point");

		Dart first = this->add_topology_element();	// First edge of the face that will fill the hole
		phi2_sew(d, first);							// 2-sew the new edge to the hole

		Dart d_next = d;							// Turn around the hole
		Dart d_phi1;								// to complete the face
		do
		{
			do
			{
				d_phi1 = this->phi1(d_next); // Search and put in d_next
				d_next = phi2(d_phi1); // the next dart of the hole
			} while (d_next != d_phi1 && d_phi1 != d);

			if (d_phi1 != d)
			{
				Dart next = this->Inherit::split_vertex_topo(first);	// Add a vertex into the built face
				phi2_sew(d_next, next);						// and 2-sew the face to the hole
			}
		} while (d_phi1 != d);

		return first;
	}

	/*!
	 * \brief Close the map by inserting faces in its holes and update the embedding of incident cells.
	 * This method is used to close a CMap2 that has been build through the 2-sewing of 1-faces.
	 * If the map has Dart, Vertex, Edge, Face or Volume attributes,
	 * the embedding of the inserted faces and incident cells are automatically updated.
	 * More precisely :
	 *  - Vertex, Edge and Volume attributes are copied, if needed, from incident cells.
	 * If the indexation of embedding was unique, the closed map is well embedded.
	 */
	// The template parameter is a hack needed to compile the class CMap2_T<DefaultMapTraits, CMap3Type<DefaultMapTraits>> with MSVC. Otherwise calling boundary_mark leads to an error.

	/*******************************************************************************
	 * Connectivity information
	 *******************************************************************************/

	inline uint32 degree(Vertex v) const
	{
		return this->nb_darts_of_orbit(v);
	}

	inline uint32 codegree(Edge e) const
	{
		if (this->phi1(e.dart) == e.dart)
			return 1;
		else
			return 2;
	}

	inline uint32 degree(Edge e) const
	{
		if (this->is_incident_to_boundary(e))
			return 1;
		else
			return 2;
	}

	inline uint32 codegree(Face f) const
	{
		return Inherit::codegree(f);
	}

	inline uint32 degree(Face) const
	{
		return 1;
	}

	inline uint32 codegree(Volume v) const
	{
		uint32 result = 0;
		foreach_incident_face(v, [&result] (Face) { ++result; });
		return result;
	}

	/**
	 * @brief Check if the given darts form a simple closed oriented path within a single Volume
	 * @param path : a vector of darts
	 * @return
	 */
	bool simple_closed_oriented_path(const std::vector<Dart>& path)
	{
		DartMarkerStore dm(*this);
		Dart prev = path.back();
		for (Dart d : path)
		{
			if (dm.is_marked(d))
				return false;
			dm.mark_orbit(Vertex(d));

			if (!this->same_cell(Vertex(d), Vertex(this->phi1(prev))))
				return false;

			prev = d;
		}
		return true;
	}

	/*******************************************************************************
	 * Boundary information
	 *******************************************************************************/

	bool is_adjacent_to_boundary(Boundary c)
	{
		CGOGN_CHECK_CONCRETE_TYPE;

		bool result = false;
		foreach_dart_of_orbit(c, [this, &result] (Dart d)
		{
			if (this->is_boundary(phi2(d))) { result = true; return false; }
			return true;
		});
		return result;
	}

	template <Orbit ORBIT>
	inline bool is_boundary_cell(Cell<ORBIT> c) const
	{
		switch (ORBIT)
		{
			case Orbit::DART: return this->is_boundary(c.dart); break;
			case Orbit::PHI1: return this->is_boundary(c.dart); break;
			case Orbit::PHI2: return false; break;
			case Orbit::PHI21: return false; break;
			case Orbit::PHI1_PHI2: return false; break;
			case Orbit::PHI1_PHI3:
			case Orbit::PHI2_PHI3:
			case Orbit::PHI21_PHI31:
			case Orbit::PHI1_PHI2_PHI3:
			default: cgogn_assert_not_reached_false("Orbit not supported in a CMap2"); break;
		}
	}

public:

	/*******************************************************************************
	* Orbits traversal                                                             *
	*******************************************************************************/

	template <Orbit ORBIT, typename FUNC>
	inline void foreach_dart_of_orbit(Cell<ORBIT> c, const FUNC& f) const
	{
		static_assert(is_func_parameter_same<FUNC, Dart>::value, "Wrong function parameter type");
		static_assert(ORBIT == Orbit::DART || ORBIT == Orbit::PHI1 || ORBIT == Orbit::PHI2 ||
					  ORBIT == Orbit::PHI1_PHI2 || ORBIT == Orbit::PHI21,
					  "Orbit not supported in a CMap2");

		switch (ORBIT)
		{
			case Orbit::DART: f(c.dart); break;
			case Orbit::PHI1: this->foreach_dart_of_PHI1(c.dart, f); break;
			case Orbit::PHI2: foreach_dart_of_PHI2(c.dart, f); break;
			case Orbit::PHI21: foreach_dart_of_PHI21(c.dart, f); break;
			case Orbit::PHI1_PHI2: foreach_dart_of_PHI1_PHI2(c.dart, f); break;
			case Orbit::PHI2_PHI3:
			case Orbit::PHI1_PHI3:
			case Orbit::PHI21_PHI31:
			case Orbit::PHI1_PHI2_PHI3:
			default: cgogn_assert_not_reached("Orbit not supported in a CMap2"); break;
		}
	}



	/**
	 * @brief check if embedding of map is also embedded in this (create if not). Used by merge method
	 * @param map
	 */
	void merge_check_embedding(const Self& map)
	{
		const static auto create_embedding = [=] (Self* map_ptr, Orbit orb)
		{
			switch (orb)
			{
				case Orbit::DART: map_ptr->template create_embedding<Orbit::DART>(); break;
				case Orbit::PHI1: map_ptr->template create_embedding<Orbit::PHI1>(); break;
				case Orbit::PHI2: map_ptr->template create_embedding<Orbit::PHI2>(); break;
				case Orbit::PHI21: map_ptr->template create_embedding<Orbit::PHI21>(); break;
				case Orbit::PHI1_PHI2: map_ptr->template create_embedding<Orbit::PHI1_PHI2>(); break;
				default: break;
			}
		};

		for (Orbit orb : { DART, PHI1, PHI2, PHI21, PHI1_PHI2 })
			if (!this->is_embedded(orb) && map.is_embedded(orb))
				create_embedding(this, orb);
	}

	/**
	 * @brief ensure all cells (introduced while merging) are embedded.
	 * @param first index of first dart to scan
	 */
	void merge_finish_embedding(uint32 first)
	{
		const static auto new_orbit_embedding = [=] (Self* map, Dart d, cgogn::Orbit orb)
		{
			switch (orb)
			{
				case Orbit::DART: map->new_orbit_embedding(Cell<Orbit::DART>(d)); break;
				case Orbit::PHI1: map->new_orbit_embedding(Cell<Orbit::PHI1>(d)); break;
				case Orbit::PHI2: map->new_orbit_embedding(Cell<Orbit::PHI2>(d)); break;
				case Orbit::PHI21: map->new_orbit_embedding(Cell<Orbit::PHI21>(d)); break;
				case Orbit::PHI1_PHI2: map->new_orbit_embedding(Cell<Orbit::PHI1_PHI2>(d)); break;
				default: break;
			}
		};

		for (uint32 j = first, end = this->topology_.end(); j != end; this->topology_.next(j))
		{
			for (Orbit orb : { DART, PHI1, PHI2, PHI21, PHI1_PHI2 })
			{
				if (this->is_embedded(orb))
				{
					if (!this->is_boundary(Dart(j)) && (*this->embeddings_[orb])[j] == INVALID_INDEX)
						new_orbit_embedding(this, Dart(j), orb);
				}
			}
		}
	}
};


} // namespace topo

} // namespace cgogn

#endif // CGOGN_CORE_CMAP_CMAP2_H_
