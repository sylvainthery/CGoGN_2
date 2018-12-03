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

#ifndef CGOGN_CORE_CMAP_CMAP1_H_
#define CGOGN_CORE_CMAP_CMAP1_H_

#include <cgogn/core/cmap/cmap0.h>

namespace cgogn
{

template <typename MAP_TYPE>
class CMap1_T : public CMap0_T<MAP_TYPE>
{
public:

	static const uint8 DIMENSION = 1;
	static const uint8 PRIM_SIZE = 1;

	using MapType = MAP_TYPE ;
	using Inherit = CMap0_T<MAP_TYPE>;
	using Self = CMap1_T<MAP_TYPE>;

	friend class MapBase<MAP_TYPE>;
	friend class DartMarker_T<Self>;
	friend class cgogn::DartMarkerStore<Self>;

	using CDart  = typename Inherit::Vertex;
	using Vertex = typename Inherit::Vertex;
	using Edge = Cell<Orbit::DART>;
	using Face = Cell<Orbit::PHI1>;

	using Boundary = Vertex;
	using ConnectedComponent = Face;

	template <typename T>
	using ChunkArrayContainer = typename Inherit::template ChunkArrayContainer<T>;
	using typename Inherit::ChunkArrayGen;
	template <typename T>
	using ChunkArray = typename Inherit::template ChunkArray<T>;

	template <typename T>
	using VertexAttribute = Attribute<T, Vertex::ORBIT>;
	template <typename T>
	using FaceAttribute = Attribute<T, Face::ORBIT>;

	using DartMarker = typename cgogn::DartMarker<Self>;
	using DartMarkerStore = typename cgogn::DartMarkerStore<Self>;

	template <Orbit ORBIT>
	using CellMarker = typename cgogn::CellMarker<Self, ORBIT>;
	template <Orbit ORBIT>
	using CellMarkerNoUnmark = typename cgogn::CellMarkerNoUnmark<Self, ORBIT>;
	template <Orbit ORBIT>
	using CellMarkerStore = typename cgogn::CellMarkerStore<Self, ORBIT>;

	using FilteredQuickTraversor = typename cgogn::FilteredQuickTraversor<Self>;
	using QuickTraversor = typename cgogn::QuickTraversor<Self>;
	using CellCache = typename cgogn::CellCache<Self>;
	using BoundaryCache = typename cgogn::BoundaryCache<Self>;

protected:

	ChunkArray<Dart>* phi1_;
	ChunkArray<Dart>* phi_1_;

	void init()
	{
		phi1_ = this->topology_.template add_chunk_array<Dart>("phi1");
		phi_1_ = this->topology_.template add_chunk_array<Dart>("phi_1");
	}

public:

	CMap1_T() : Inherit()
	{
		init();
	}

	CGOGN_NOT_COPYABLE_NOR_MOVABLE(CMap1_T);

	~CMap1_T() override
	{}


	inline ChunkArray<Dart>& ca_phi1()
	{
		return *(phi1_);
	}


	/*!
	 * \brief Check the integrity of embedding information
	 */

	/*******************************************************************************
	 * Low-level topological operations
	 *******************************************************************************/

	/*!
	* \brief Init an newly added dart.
	* The dart is defined as a fixed point for PHI1.
	*/
	inline void init_dart(Dart d)
	{
		Inherit::init_dart(d);
		(*phi1_)[d.index] = d;
		(*phi_1_)[d.index] = d;
	}

	/**
	 * @brief Check the integrity of a dart
	 * @param d the dart to check
	 * @return true if the integrity constraints are locally statisfied
	 * PHI1 and PHI_1 are inverse relations.
	 */
	inline bool check_integrity(Dart d) const
	{
		return (phi1(phi_1(d)) == d &&
				phi_1(phi1(d)) == d);
	}

	/**
	 * @brief Check the integrity of a boundary dart
	 * @param d the dart to check
	 * @return true if the boundary constraints are locally statisfied
	 * No boundary dart is accepted.
	 */
	inline bool check_boundary_integrity(Dart d) const
	{
		return !this->is_boundary(d);
	}

	/*!
	 * \brief Link two darts with the phi1 permutation what either merge or split their orbit(s).
	 * @param d: the first dart
	 * @param e: the second dart
	 * - Before: d->f and e->g
	 * - After:  d->g and e->f
	 * Join the orbits of dart d and e if they are distinct
	 * - Starting from two cycles : d->f->...->d and e->g->...->e
	 * - It makes one cycle d->g->...->e->f->...->d
	 * If e = g then insert e in the cycle of d : d->e->f->...->d
	 * If d and e are in the same orbit of phi1, this orbit is split in two cycles.
	 * - Starting with d->g->...e->f->...->d
	 * - It makes two cycles : d->f->...->d and e->g->...->e
	 */
	void phi1_sew(Dart d, Dart e)
	{
		Dart f = phi1(d);
		Dart g = phi1(e);
		(*phi1_)[d.index] = g;
		(*phi1_)[e.index] = f;
		(*phi_1_)[g.index] = d;
		(*phi_1_)[f.index] = e;
	}

	/*!
	 * \brief Remove the successor of a given dart from its permutation
	 * @param d a dart
	 * - Before: d->e->f
	 * - After:  d->f and e->e
	 */
	void phi1_unsew(Dart d)
	{
		Dart e = phi1(d);
		Dart f = phi1(e);
		(*phi1_)[d.index] = f;
		(*phi1_)[e.index] = e;
		(*phi_1_)[f.index] = d;
		(*phi_1_)[e.index] = e;
	}

	/*******************************************************************************
	 * Basic topological operations
	 *******************************************************************************/

public:

	/*!
	 * \brief phi1
	 * @param d
	 * @return phi1(d)
	 */
	inline Dart phi1(Dart d) const
	{
		return (*phi1_)[d.index];
	}

	/*!
	 * \brief phi_1
	 * @param d
	 * @return phi_1(d)
	 */
	Dart phi_1(Dart d) const
	{
		return (*phi_1_)[d.index];
	}

	/**
	 * \brief Composition of PHI calls
	 * @param d
	 * @return The result of successive applications of PHI1 on d.
	 * The template parameter contains a sequence (Base10 encoded) of PHI indices.
	 * If N=0 the identity is used.
	 */
	template <uint64 N>
	inline Dart phi(Dart d) const
	{
		static_assert((N%10)<=1,"Composition of PHI: invalid index");
		if (N >=10)
			return this->phi1(phi<N/10>(d));

		if (N == 1)
			return this->phi1(d);

		return d;
	}



	/*******************************************************************************
	* Orbits traversal                                                             *
	*******************************************************************************/
	template <typename FUNC>
	inline void foreach_dart_of_PHI1(Dart d, const FUNC& f) const
	{
		Dart it = d;
		do
		{
			if (!internal::void_to_true_binder(f, it))
				break;
			it = phi1(it);
		} while (it != d);
	}


};

struct CMap1Type
{
	using TYPE = CMap1_T<CMap1Type>;
};

using CMap1 = CMap1_T<CMap1Type>;

#if defined(CGOGN_USE_EXTERNAL_TEMPLATES) && (!defined(CGOGN_CORE_EXTERNAL_TEMPLATES_CPP_))
extern template class CGOGN_CORE_API CMap1_T<CMap1Type>;
extern template class CGOGN_CORE_API DartMarker<CMap1>;
extern template class CGOGN_CORE_API DartMarkerStore<CMap1>;
extern template class CGOGN_CORE_API DartMarkerNoUnmark<CMap1>;
extern template class CGOGN_CORE_API CellMarker<CMap1, CMap1::Vertex::ORBIT>;
extern template class CGOGN_CORE_API CellMarker<CMap1, CMap1::Face::ORBIT>;
extern template class CGOGN_CORE_API CellMarkerNoUnmark<CMap1, CMap1::Vertex::ORBIT>;
extern template class CGOGN_CORE_API CellMarkerNoUnmark<CMap1, CMap1::Face::ORBIT>;
extern template class CGOGN_CORE_API CellMarkerStore<CMap1, CMap1::Vertex::ORBIT>;
extern template class CGOGN_CORE_API CellMarkerStore<CMap1, CMap1::Face::ORBIT>;
extern template class CGOGN_CORE_API QuickTraversor<CMap1>;
#endif // defined(CGOGN_USE_EXTERNAL_TEMPLATES) && (!defined(CGOGN_CORE_EXTERNAL_TEMPLATES_CPP_))

} // namespace cgogn

#endif // CGOGN_CORE_CMAP_CMAP1_H_
