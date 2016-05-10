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

#include <gtest/gtest.h>

#include <cgogn/core/cmap/cmap2_builder.h>

namespace cgogn
{

#define NB_MAX 100

/*!
 * \brief The CMap2Test class implements tests on embedded CMap2
 * It contains a CMap2 to which vertex, edge, face and volume attribute
 * are added to enforce the indexation mechanism in cell traversals.
 *
 * Note that pure topological operations have already been tested,
 * in CMap2TopoTest, thus only the indexation mechanism used for the
 * embedding of cells is tested here.
 */
class CMap2Test : public ::testing::Test
{

public:

	struct MiniMapTraits
	{
		static const uint32 CHUNK_SIZE = 16;
	};

	using testCMap2 = CMap2<MiniMapTraits>;
	using MapBuilder = CMap2Builder_T<MiniMapTraits>;
	using CDart = testCMap2::CDart;
	using Vertex = testCMap2::Vertex;
	using Edge = testCMap2::Edge;
	using Face = testCMap2::Face;
	using Volume = testCMap2::Volume;

protected:

	testCMap2 cmap_;

	/*!
	 * \brief A vector of darts on which the methods are tested.
	 */
	std::vector<Dart> darts_;

	CMap2Test()
	{
		darts_.reserve(NB_MAX);
		std::srand(uint32(std::time(0)));

		cmap_.add_attribute<int32, CDart::ORBIT>("darts");
		cmap_.add_attribute<int32, Vertex::ORBIT>("vertices");
		cmap_.add_attribute<int32, Edge::ORBIT>("edges");
		cmap_.add_attribute<int32, Face::ORBIT>("faces");
		cmap_.add_attribute<int32, Volume::ORBIT>("volumes");
	}

	/*!
	 * \brief Generate a random set of faces and put them in darts_
	 * \return The total number of added vertices.
	 * The face size ranges from 1 to 10.
	 * A random dart of each face is put in the darts_ array.
	 */
	uint32 add_faces(uint32 n)
	{
		darts_.clear();
		uint32 count = 0u;
		for (uint32 i = 0u; i < n; ++i)
		{
			uint32 m = 1u + std::rand() % 10u;
			Dart d = cmap_.add_face(m).dart;
			count += m;

			m = std::rand() % 10u;
			while (m-- > 0u) d = cmap_.phi1(d);

			darts_.push_back(d);
		}
		return count;
	}

	/*!
	 * \brief Generate a closed surface from the set of faces in darts_
	 */
	void add_closed_surfaces()
	{
		darts_.clear();
		MapBuilder mbuild(cmap_);

		// Generate NB_MAX random 1-faces (without boundary)
		for (uint32 i = 0u; i < NB_MAX; ++i)
		{
			uint32 n = 1u + std::rand() % 10u;
			Dart d = mbuild.add_face_topo_parent(n);
			darts_.push_back(d);
		}
		// Sew some pairs of edges
		for (uint32 i = 0u; i < 3u * NB_MAX; ++i)
		{
			Dart e1 = darts_[std::rand() % NB_MAX];
			uint32 n = std::rand() % 10u;
			while (n-- > 0u) e1 = cmap_.phi1(e1);

			Dart e2 = darts_[std::rand() % NB_MAX];
			n = std::rand() % 10u;
			while (n-- > 0u) e2 = cmap_.phi1(e2);

			n = 1 + std::rand() % 3u;
			while (n-- > 0u && cmap_.phi2(e1) == e1 && cmap_.phi2(e2) == e2 && e2 != e1)
			{
				mbuild.phi2_sew(e2, e1);
				e1 = cmap_.phi1(e1);
				e2 = cmap_.phi_1(e2);
			}
		}
		// Close the map (remove remaining boundary)
		cmap_.foreach_dart([&] (Dart d)
		{
			if (cmap_.phi2(d) == d) mbuild.close_hole_topo(d);
		});
		// Embed the map
		cmap_.foreach_dart([&] (Dart d)
		{
			if (!cmap_.is_boundary(d))
				mbuild.new_orbit_embedding(CDart(d));
		});
		cmap_.foreach_cell<FORCE_DART_MARKING>([&] (Vertex v)
		{
			mbuild.new_orbit_embedding(v);
		});
		cmap_.foreach_cell<FORCE_DART_MARKING>([&] (Edge e)
		{
			mbuild.new_orbit_embedding(e);
		});
		cmap_.foreach_cell<FORCE_DART_MARKING>([&] (Face f)
		{
			mbuild.new_orbit_embedding(f);
		});
		cmap_.foreach_cell<FORCE_DART_MARKING>([&] (Volume w)
		{
			mbuild.new_orbit_embedding(w);
		});
	}
};

/*!
 * \brief The random generated maps used in the tests are sound.
 */
TEST_F(CMap2Test, random_map_generators)
{
	add_faces(NB_MAX);
	EXPECT_TRUE(cmap_.check_map_integrity());

	add_closed_surfaces();
	EXPECT_TRUE(cmap_.check_map_integrity());
}

/*!
 * \brief Adding faces preserves the cell indexation
 */
TEST_F(CMap2Test, add_face)
{
	uint32 count_vertices = add_faces(NB_MAX);

	EXPECT_EQ(cmap_.nb_cells<Vertex::ORBIT>(), count_vertices);
	EXPECT_EQ(cmap_.nb_cells<Edge::ORBIT>(), count_vertices);
	EXPECT_EQ(cmap_.nb_cells<Face::ORBIT>(), NB_MAX);
	EXPECT_EQ(cmap_.nb_cells<Volume::ORBIT>(), NB_MAX);
	EXPECT_TRUE(cmap_.check_map_integrity());
}

/*!
 * \brief Cutting edges preserves the cell indexation
 */
TEST_F(CMap2Test, cut_edge)
{
	add_closed_surfaces();

	for (Dart d : darts_)
		cmap_.cut_edge(Edge(d));

	EXPECT_TRUE(cmap_.check_map_integrity());
}

/*!
 * \brief Cutting faces preserves the cell indexation
 */
TEST_F(CMap2Test, cut_face)
{
	add_closed_surfaces();

	for (Dart d : darts_)
	{
		if (cmap_.codegree(Face(d)) > 1u)
		{
			Dart e = d; // find a second dart in the face of d (distinct from d)
			uint32 i = std::rand() % 10u;
			while (i-- > 0u) e = cmap_.phi1(e);
			if (e == d) e = cmap_.phi1(e);

			cmap_.cut_face(Vertex(d), Vertex(e));
		}
	}
	EXPECT_TRUE(cmap_.check_map_integrity());
}


TEST_F(CMap2Test, compact_map)
{

	testCMap2::VertexAttribute<int32> att_v = cmap_.get_attribute<int32, Vertex::ORBIT>("vertices");
	testCMap2::EdgeAttribute<int32> att_e = cmap_.get_attribute<int32, Edge::ORBIT>("edges");
	testCMap2::FaceAttribute<int32> att_f = cmap_.get_attribute<int32, Face::ORBIT>("faces");

	for (int32 i=0; i<100; ++i)
	{
		Face f = cmap_.add_face(5);
		int32 ec=0;
		cmap_.foreach_incident_edge(f, [&] (Edge e)
		{
			ec++;
			att_e[e]=100*i+ec;
			att_v[Vertex(e.dart)]=1000*i+ec;
		});
		att_f[i]=10*i;
		darts_.push_back(f.dart);
	}

	for (int32 i=0; i<100; i+=2)
	{
		Edge e(cmap_.phi1(darts_[i]));
		cmap_.collapse_edge(e);
		e = Edge(cmap_.phi1(darts_[i]));
		cmap_.collapse_edge(e);
	}

//	cmap_.foreach_cell([&] (Face f)
//	{
//		std::cout << "FACE:" <<att_f[f]<< std::endl<< "  Vertices: ";
//		cmap_.foreach_incident_vertex(f, [&] (Vertex v)
//		{
//			std::cout << att_v[v]<< " / ";
//		});
//		std::cout << std::endl<< "  Edges: ";
//		cmap_.foreach_incident_edge(f, [&] (Edge e)
//		{
//			std::cout << att_e[e]<< " / ";
//		});
//		std::cout << std::endl;
//	});


	EXPECT_TRUE(cmap_.check_map_integrity());
	cmap_.compact();
	EXPECT_TRUE(cmap_.check_map_integrity());


	EXPECT_EQ(cmap_.get_topology_container().size(),cmap_.get_topology_container().end());

	EXPECT_EQ(cmap_.get_const_attribute_container<Vertex::ORBIT>().size(),
			  cmap_.get_const_attribute_container<Vertex::ORBIT>().end());

	EXPECT_EQ(cmap_.get_const_attribute_container<Edge::ORBIT>().size(),
			  cmap_.get_const_attribute_container<Edge::ORBIT>().end());


//	std::cout << "TOPO SIZE:"<< cmap_.get_topology_container().size() << " / END:"<<  cmap_.get_topology_container().end() << std::endl;

//		std::cout << "VERT SIZE:"<< cmap_.get_attribute_container<Vertex::ORBIT>().size() << " / END:"<<  cmap_.get_attribute_container<Vertex::ORBIT>().end() << std::endl;


//	cmap_.foreach_cell([&] (Face f)
//	{
//		std::cout << "FACE:" <<att_f[f]<< std::endl<< "  Vertices: ";
//		cmap_.foreach_incident_vertex(f, [&] (Vertex v)
//		{
//			std::cout << att_v[v]<< " / ";
//		});
//		std::cout << std::endl<< "  Edges: ";
//		cmap_.foreach_incident_edge(f, [&] (Edge e)
//		{
//			std::cout << att_e[e]<< " / ";
//		});
//		std::cout << std::endl;
//	});

}

TEST_F(CMap2Test, merge_map)
{
	using CDart = testCMap2::CDart;
	using Vertex = testCMap2::Vertex;
	using Edge = testCMap2::Edge;
	using Face = testCMap2::Face;
	using Volume = testCMap2::Volume;


	testCMap2 map1;
//	map1.add_attribute<int32, CDart::ORBIT>("darts");
	testCMap2::VertexAttribute<int32> att1_v = map1.add_attribute<int32, Vertex::ORBIT>("vertices");
//	map1.add_attribute<int32, Edge::ORBIT>("edges");
	testCMap2::FaceAttribute<int32> att1_f = map1.add_attribute<int32, Face::ORBIT>("faces");
//	map1.add_attribute<int32, Volume::ORBIT>("volumes");

	testCMap2 map2;
	testCMap2::Attribute<int32,CDart::ORBIT> att2_d = map2.add_attribute<int32, CDart::ORBIT>("darts");
//	map2.add_attribute<int32, Vertex::ORBIT>("vertices");
	testCMap2::EdgeAttribute<int32> att2_e = map2.add_attribute<int32, Edge::ORBIT>("edges");
//	map2.add_attribute<int32, Face::ORBIT>("faces");
	testCMap2::VolumeAttribute<int32> att2_w = map2.add_attribute<int32, Volume::ORBIT>("volumes");

	for (int32 i=0; i<5; ++i)
	{
		Face f = map1.add_face(4);
		int32 ec=0;
		map1.foreach_incident_vertex(f, [&] (Vertex v)
		{
			ec++;
			att1_v[v]=1000*i+ec;
		});
		att1_f[i]=10*i;
	}

	for (int32 i=0; i<5; ++i)
	{
		Face f = map2.add_face(3);
		int32 ec=0;
		map2.foreach_incident_edge(f, [&] (Edge e)
		{
			ec++;
			att2_e[e]=100*i+ec;
		});
	}

	map1.merge(map2);
}

#undef NB_MAX

} // namespace cgogn
