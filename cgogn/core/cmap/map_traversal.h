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

#ifndef CGOGN_CORE_CMAP_MAP_BASE_TRAV_H_
#define CGOGN_CORE_CMAP_MAP_BASE_TRAV_H_

#include <cgogn/core/cmap/map_base.h>

enum TraversalStrategy
{
	AUTO = 0,
	FORCE_DART_MARKING,
	FORCE_CELL_MARKING
};

namespace cgogn
{


	/**
	 * \brief apply a function on each dart of the map (including boundary darts)
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @param f a callable
	 */
	template <typename CMAP, typename FUNC>
	inline void foreach_dart(CMAP& cmap, const FUNC& f)
	{
		static_assert(is_func_parameter_same<FUNC, Dart>::value, "foreach_dart: given function should take a Dart as parameter");

		for (Dart it = cmap.all_begin(), last = cmap.all_end(); it != last; cmap.all_next(it))
		{
			if (!internal::void_to_true_binder(f, it))
				break;
		}
	}

	/**
	 * \brief apply a function in parallel on each dart of the map (including boundary darts)
	 * @tparam FUNC type of the callable
	 * @param f a callable
	 */
	template <typename CMAP, typename FUNC>
	inline void parallel_foreach_dart(CMAP& cmap, const FUNC& f)
	{
		static_assert(is_func_parameter_same<FUNC, Dart>::value, "parallel_foreach_dart: given function should take a Dart as parameter");

		using Future = std::future<typename std::result_of<FUNC(Dart)>::type>;
		using VecDarts = std::vector<Dart>;

		ThreadPool* thread_pool = cgogn::thread_pool();
		uint32 nb_workers = thread_pool->nb_workers();
		if (nb_workers == 0)
			return foreach_dart(f);

		std::array<std::vector<VecDarts*>, 2> dart_buffers;
		std::array<std::vector<Future>, 2> futures;
		dart_buffers[0].reserve(nb_workers);
		dart_buffers[1].reserve(nb_workers);
		futures[0].reserve(nb_workers);
		futures[1].reserve(nb_workers);

		Buffers<Dart>* dbuffs = cgogn::dart_buffers();

		uint32 i = 0u; // buffer id (0/1)
		uint32 j = 0u; // thread id (0..nb_workers)
		Dart it = cmap.all_begin();
		Dart last = cmap.all_end();

		while (it != last)
		{
			dart_buffers[i].push_back(dbuffs->buffer());
			cgogn_assert(dart_buffers[i].size() <= nb_workers);
			std::vector<Dart>& darts = *dart_buffers[i].back();
			darts.reserve(PARALLEL_BUFFER_SIZE);
			for (unsigned k = 0u; k < PARALLEL_BUFFER_SIZE && it.index < last.index; ++k)
			{
				darts.push_back(it);
				cmap.all_next(it);
			}

			futures[i].push_back(thread_pool->enqueue([&darts, &f] ()
			{
				for (auto d : darts)
					f(d);
			}));

			// next thread
			if (++j == nb_workers)
			{	// again from 0 & change buffer
				j = 0;
				i = (i+1u) % 2u;
				for (auto& fu : futures[i])
					fu.wait();
				for (auto& b : dart_buffers[i])
					dbuffs->release_buffer(b);
				futures[i].clear();
				dart_buffers[i].clear();
			}
		}

		// clean all at end
		for (auto& fu : futures[0u])
			fu.wait();
		for (auto& b : dart_buffers[0u])
			dbuffs->release_buffer(b);
		for (auto& fu : futures[1u])
			fu.wait();
		for (auto& b : dart_buffers[1u])
			dbuffs->release_buffer(b);
	}

	/**
	 * \brief apply a function on each cell of the map (boundary cells excluded)
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @param f a callable
	 */
	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC>
	inline void foreach_cell(CMAP& cmap, const FUNC& f)
	{
		using CellType = func_parameter_type<FUNC>;

		foreach_cell<STRATEGY>(cmap, f, [] (CellType) { return true; });
	}

	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC>
	inline void foreach_cell(CMAP& cmap, const FUNC& f, const AllCellsFilter&)
	{
		using CellType = func_parameter_type<FUNC>;

		foreach_cell<STRATEGY>(cmap, f, [] (CellType) { return true; });
	}

	/**
	 * \brief apply a function in parallel on each cell of the map (boundary cells excluded)
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * @tparam FUNC type of the callable
	 * @param f a callable
	 */
	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC>
	inline void parallel_foreach_cell(CMAP& cmap, const FUNC& f)
	{
		using CellType = func_parameter_type<FUNC>;

		parallel_foreach_cell<STRATEGY>(cmap, f, [] (CellType) { return true; });
	}

	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC>
	inline void parallel_foreach_cell(CMAP& cmap, const FUNC& f, const AllCellsFilter&)
	{
		using CellType = func_parameter_type<FUNC>;

		parallel_foreach_cell<STRATEGY>(cmap, f, [] (CellType) { return true; });
	}

	/**
	 * \brief apply a function on each cell of the map (boundary cells excluded)
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the given FilterFunction (CellType -> bool) are processed
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @tparam FilterFunction type of the cell filtering function (CellType -> bool)
	 * @param f a callable
	 * @param filter a cell filtering function
	 */
	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC, typename FilterFunction>
	inline auto foreach_cell(CMAP& cmap, const FUNC& f, const FilterFunction& filter)
		-> typename std::enable_if<
			is_func_return_same<FilterFunction, bool>::value  &&
			is_func_parameter_same<FilterFunction, func_parameter_type<FUNC>>::value
		   >::type
	{
		using CellType = func_parameter_type<FUNC>;

		switch (STRATEGY)
		{
			case FORCE_DART_MARKING :
				foreach_cell_dart_marking(cmap,f, filter);
				break;
			case FORCE_CELL_MARKING :
				foreach_cell_cell_marking(cmap, f, filter);
				break;
			case AUTO :
				if (cmap.is_embedded<CellType>())
					foreach_cell_cell_marking(cmap, f, filter);
				else
					foreach_cell_dart_marking(cmap, f, filter);
				break;
		}
	}

	/**
	 * \brief apply a function in parallel on each cell of the map (boundary cells excluded)
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the given FilterFunction (CellType -> bool) are processed
	 * @tparam FUNC type of the callable
	 * @tparam FilterFunction type of the cell filtering function (CellType -> bool)
	 * @param f a callable
	 * @param filter a cell filtering function
	 */
	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC, typename FilterFunction>
	inline auto parallel_foreach_cell(CMAP& cmap, const FUNC& f, const FilterFunction& filter)
		-> typename std::enable_if<
			is_func_return_same<FilterFunction, bool>::value &&
			is_func_parameter_same<FilterFunction, func_parameter_type<FUNC>>::value
		   >::type
	{
		using CellType = func_parameter_type<FUNC>;

		switch (STRATEGY)
		{
			case FORCE_DART_MARKING :
				parallel_foreach_cell_dart_marking(cmap, f, filter);
				break;
			case FORCE_CELL_MARKING :
				parallel_foreach_cell_cell_marking(cmap, f, filter);
				break;
			case AUTO :
				if (cmap.is_embedded<CellType>())
					parallel_foreach_cell_cell_marking(cmap, f, filter);
				else
					parallel_foreach_cell_dart_marking(cmap, f, filter);
				break;
		}
	}

	/**
	 * \brief apply a function on each cell of the map (boundary cells excluded)
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the filter function of the corresponding CellType within the given CellFilters object are processed
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @param f a callable
	 * @param filters a CellFilters object (contains a filtering function for each CellType)
	 */
	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC>
	inline void foreach_cell(CMAP& cmap, const FUNC& f, const CellFilters& filters) const
	{
		using CellType = func_parameter_type<FUNC>;

		if ((filters.filtered_cells() & orbit_mask<CellType>()) == 0u)
			cgogn_log_warning("foreach_cell") << "Using a CellFilter for a non-filtered CellType";
z
		foreach_cell<STRATEGY>(f, [&filters] (CellType c) { return filters.filter(c); });
	}

	/**
	 * \brief apply a function in parallel on each cell of the map (boundary cells excluded)
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the filter function of the corresponding CellType within the given Filters object are processed
	 * @tparam FUNC type of the callable
	 * @param f a callable
	 * @param filters a CellFilters object (contains a filtering function for each CellType)
	 */
	template <TraversalStrategy STRATEGY = TraversalStrategy::AUTO, typename CMAP, typename FUNC>
	inline void parallel_foreach_cell(CMAP& cmap, const FUNC& f, const CellFilters& filters)
	{
		using CellType = func_parameter_type<FUNC>;

		if ((filters.filtered_cells() & orbit_mask<CellType>()) == 0u)
			cgogn_log_warning("foreach_cell") << "Using a CellFilter for a non-filtered CellType";

		parallel_foreach_cell<STRATEGY>(cmap, f, [&filters] (CellType c) { return filters.filter(c); });
	}

	/**
	 * \brief apply a function on each cell of the map that is provided by the given Traversor object
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @tparam Traversor type of the CellTraversor object (inherits from CellTraversor)
	 * @param f a callable
	 * @param t a Traversor object
	 */
	template <typename FUNC, typename Traversor>
	inline auto foreach_cell(const FUNC& f, const Traversor& t) const
		-> typename std::enable_if<std::is_base_of<CellTraversor, Traversor>::value>::type
	{
		using CellType = func_parameter_type<FUNC>;

		if (!t.template is_traversed<CellType>())
			cgogn_log_warning("foreach_cell") << "Using a CellTraversor for a non-traversed CellType";

		for (typename Traversor::const_iterator it = t.template begin<CellType>(), end = t.template end<CellType>(); it != end; ++it)
			if (!internal::void_to_true_binder(f, CellType(*it)))
				break;
	}

	/**
	 * \brief apply a function in parallel on each cell of the map that is provided by the given Traversor object
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * @tparam FUNC type of the callable
	 * @tparam Traversor type of the CellTraversor object (inherits from CellTraversor)
	 * @param f a callable
	 * @param t a Traversor object
	 */
	template <typename FUNC, typename Traversor>
	inline auto parallel_foreach_cell(const FUNC& f, const Traversor& t)
		-> typename std::enable_if<std::is_base_of<CellTraversor, Traversor>::value>::type
	{
		using CellType = func_parameter_type<FUNC>;

		using VecCell = std::vector<CellType>;
		using Future = std::future<typename std::result_of<FUNC(CellType)>::type>;

		if (!t.template is_traversed<CellType>())
			cgogn_log_warning("foreach_cell") << "Using a CellTraversor for a non-traversed CellType";

		ThreadPool* thread_pool = cgogn::thread_pool();
		uint32 nb_workers = thread_pool->nb_workers();
		if (nb_workers == 0)
			return foreach_cell(f, t);

		std::array<std::vector<VecCell*>, 2> cells_buffers;
		std::array<std::vector<Future>, 2> futures;
		cells_buffers[0].reserve(nb_workers);
		cells_buffers[1].reserve(nb_workers);
		futures[0].reserve(nb_workers);
		futures[1].reserve(nb_workers);

		Buffers<Dart>* dbuffs = cgogn::dart_buffers();

		uint32 i = 0u; // buffer id (0/1)
		uint32 j = 0u; // thread id (0..nb_workers)
		auto it = t.template begin<CellType>();
		const auto it_end = t.template end<CellType>();
		while(it != it_end)
		{
			// fill buffer
			cells_buffers[i].push_back(dbuffs->template cell_buffer<CellType>());
			VecCell& cells = *cells_buffers[i].back();
			cells.reserve(PARALLEL_BUFFER_SIZE);
			for (unsigned k = 0u; k < PARALLEL_BUFFER_SIZE && (it != it_end); ++k)
			{
				cells.push_back(CellType(*it));
				++it;
			}
			// launch thread
			futures[i].push_back(thread_pool->enqueue([&cells, &f] ()
			{
				for (auto c : cells)
					f(c);
			}));
			// next thread
			if (++j == nb_workers)
			{	// again from 0 & change buffer
				j = 0;
				i = (i+1u) % 2u;
				for (auto& fu : futures[i])
					fu.wait();
				for (auto& b : cells_buffers[i])
					dbuffs->release_cell_buffer(b);
				futures[i].clear();
				cells_buffers[i].clear();
			}
		}

		// clean all at end
		for (auto& fu : futures[0u])
			fu.wait();
		for (auto& b : cells_buffers[0u])
			dbuffs->release_cell_buffer(b);
		for (auto& fu : futures[1u])
			fu.wait();
		for (auto& b : cells_buffers[1u])
			dbuffs->release_cell_buffer(b);
	}

	/**
	 * \brief apply a function on each cell of the map (boundary cells excluded) using a DartMarker
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the given FilterFunction (CellType -> bool) are processed
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @tparam FilterFunction type of the cell filtering function (CellType -> bool)
	 * @param f a callable
	 * @param filter a cell filtering function
	 */
	template <typename CMAP, typename FUNC, typename FilterFunction>
	inline void foreach_cell_dart_marking(CMAP& cmap, const FUNC& f, const FilterFunction& filter)
	{
		using CellType = func_parameter_type<FUNC>;

		DartMarker dm(cmap);
		for (Dart it = cmap.begin(), last = cmap.end(); it.index < last.index; cmap.next(it))
		{
			if (!dm.is_marked(it))
			{
				const CellType c(it);
				dm.mark_orbit(c);
				if (filter(c) && !internal::void_to_true_binder(f, c))
					break;
			}
		}
	}

	/**
	 * \brief apply a function on each cell of the map (boundary cells excluded) using a DartMarker
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the given FilterFunction (CellType -> bool) are processed
	 * @tparam FUNC type of the callable
	 * @tparam FilterFunction type of the cell filtering function (CellType -> bool)
	 * @param f a callable
	 * @param filter a cell filtering function
	 */
	template <typename CMAP, typename FUNC, typename FilterFunction>
	inline void parallel_foreach_cell_dart_marking(CMAP& cmap, const FUNC& f, const FilterFunction& filter) const
	{
		using CellType = func_parameter_type<FUNC>;

		using VecCell = std::vector<CellType>;
		using Future = std::future<typename std::result_of<FUNC(CellType)>::type>;

		ThreadPool* thread_pool = cgogn::thread_pool();
		uint32 nb_workers = thread_pool->nb_workers();
		if (nb_workers == 0)
			return foreach_cell_dart_marking(f, filter);

		std::array<std::vector<VecCell*>, 2> cells_buffers;
		std::array<std::vector<Future>, 2> futures;
		cells_buffers[0].reserve(nb_workers);
		cells_buffers[1].reserve(nb_workers);
		futures[0].reserve(nb_workers);
		futures[1].reserve(nb_workers);

		Buffers<Dart>* dbuffs = cgogn::dart_buffers();

		DartMarker<CMAP> dm(cmap);
		Dart it = cmap.begin();
		Dart last = cmap.end();

		uint32 i = 0u; // buffer id (0/1)
		uint32 j = 0u; // thread id (0..nb_workers)
		while (it.index < last.index)
		{
			// fill buffer
			cells_buffers[i].push_back(dbuffs->template cell_buffer<CellType>());
			VecCell& cells = *cells_buffers[i].back();
			cells.reserve(PARALLEL_BUFFER_SIZE);
			for (unsigned k = 0u; k < PARALLEL_BUFFER_SIZE && it.index < last.index; )
			{
				if (!dm.is_marked(it))
				{
					CellType c(it);
					dm.mark_orbit(c);
					if (filter(c))
					{
						cells.push_back(c);
						++k;
					}
				}
				cmap.next(it);
			}
			//launch thread
			futures[i].push_back(thread_pool->enqueue([&cells, &f] ()
			{
				for (auto c : cells)
					f(c);
			}));
			// next thread
			if (++j == nb_workers)
			{	// again from 0 & change buffer
				j = 0;
				i = (i+1u) % 2u;
				for (auto& fu : futures[i])
					fu.wait();
				for (auto& b : cells_buffers[i])
					dbuffs->release_cell_buffer(b);
				futures[i].clear();
				cells_buffers[i].clear();
			}
		}

		// clean all at end
		for (auto& fu : futures[0u])
			fu.wait();
		for (auto &b : cells_buffers[0u])
			dbuffs->release_cell_buffer(b);
		for (auto& fu : futures[1u])
			fu.wait();
		for (auto &b : cells_buffers[1u])
			dbuffs->release_cell_buffer(b);
	}

	/**
	 * \brief apply a function on each cell of the map (boundary cells excluded) using a CellMarker
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the given FilterFunction (CellType -> bool) are processed
	 * if the function returns a boolean, the traversal stops when it first returns false
	 * @tparam FUNC type of the callable
	 * @tparam FilterFunction type of the cell filtering function (CellType -> bool)
	 * @param f a callable
	 * @param filter a cell filtering function
	 */
	template <typename CMAP, typename FUNC, typename FilterFunction>
	inline void foreach_cell_cell_marking(CMAP& cmap, const FUNC& f, const FilterFunction& filter)
	{
		using CellType = func_parameter_type<FUNC>;

		CellMarker<CMAP, CellType::ORBIT> cm(cmap);
		for (Dart it = cmap.begin(), last = cmap.end(); it.index < last.index; cmap.next(it))
		{
			const CellType c(it);
			if (!cm.is_marked(c))
			{
				cm.mark(c);
				if (filter(c) && !internal::void_to_true_binder(f, c))
					break;
			}
		}
	}

	/**
	 * \brief apply a function in parallel on each cell of the map (boundary cells excluded) using a CellMarker
	 * the dimension of the traversed cells is determined based on the parameter of the given callable
	 * only cells selected by the given FilterFunction (CellType -> bool) are processed
	 * @tparam FUNC type of the callable
	 * @tparam FilterFunction type of the cell filtering function (CellType -> bool)
	 * @param f a callable
	 * @param filter a cell filtering function
	 */
	template <typename CMAP, typename FUNC, typename FilterFunction>
	inline void parallel_foreach_cell_cell_marking(CMAP& cmap, const FUNC& f, const FilterFunction& filter)
	{
		using CellType = func_parameter_type<FUNC>;
		static const Orbit ORBIT = CellType::ORBIT;

		using VecCell = std::vector<CellType>;
		using Future = std::future<typename std::result_of<FUNC(CellType)>::type>;

		ThreadPool* thread_pool = cgogn::thread_pool();
		uint32 nb_workers = thread_pool->nb_workers();
		if (nb_workers == 0)
			return foreach_cell_cell_marking(f, filter);

		std::array<std::vector<VecCell*>, 2> cells_buffers;
		std::array<std::vector<Future>, 2> futures;
		cells_buffers[0].reserve(nb_workers);
		cells_buffers[1].reserve(nb_workers);
		futures[0].reserve(nb_workers);
		futures[1].reserve(nb_workers);

		Buffers<Dart>* dbuffs = cgogn::dart_buffers();

		CellMarker<CMAP,ORBIT> cm(cmap);
		Dart it = cmap.begin();
		Dart last = cmap.end();

		uint32 i = 0u; // buffer id (0/1)
		uint32 j = 0u; // thread id (0..nb_workers)
		while (it.index < last.index)
		{
			// fill buffer
			cells_buffers[i].push_back(dbuffs->template cell_buffer<CellType>());
			VecCell& cells = *cells_buffers[i].back();
			cells.reserve(PARALLEL_BUFFER_SIZE);
			for (unsigned k = 0u; k < PARALLEL_BUFFER_SIZE && it.index < last.index; )
			{
				CellType c(it);
				if (!cm.is_marked(c))
				{
					cm.mark(c);
					if (filter(c))
					{
						cells.push_back(c);
						++k;
					}
				}
				cmap.next(it);
			}
			// launch thread
			futures[i].push_back(thread_pool->enqueue([&cells, &f] ()
			{
				for (auto c : cells)
					f(c);
			}));
			// next thread
			if (++j == nb_workers)
			{	// again from 0 & change buffer
				j = 0;
				i = (i+1u) % 2u;
				for (auto& fu : futures[i])
					fu.wait();
				for (auto& b : cells_buffers[i])
					dbuffs->release_cell_buffer(b);
				futures[i].clear();
				cells_buffers[i].clear();
			}
		}

		// clean all at end
		for (auto& fu : futures[0u])
			fu.wait();
		for (auto& b : cells_buffers[0u])
			dbuffs->release_cell_buffer(b);
		for (auto& fu : futures[1u])
			fu.wait();
		for (auto& b : cells_buffers[1u])
			dbuffs->release_cell_buffer(b);
	}


} // namespace cgogn

#endif // CGOGN_CORE_CMAP_MAP_BASE_H_
