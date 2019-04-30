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

#include <imgui.h>

#include <cgogn/rendering_pureGL/imgui_viewer.h>
#include <iostream>

#include <cgogn/core/cmap/cmap2.h>
#include <cgogn/io/map_import.h>
#include <cgogn/geometry/algos/bounding_box.h>
#include <cgogn/geometry/algos/normal.h>
#include <cgogn/rendering_pureGL/map_render.h>
#include <cgogn/rendering_pureGL/shaders/shader_simple_color.h>
#include <cgogn/rendering_pureGL/shaders/shader_flat.h>
#include <cgogn/rendering_pureGL/vbo.h>
#include <cgogn/rendering_pureGL/vbo_update.h>
#include <cgogn/geometry/algos/ear_triangulation.h>

#define DEFAULT_MESH_PATH CGOGN_STR(CGOGN_TEST_MESHES_PATH)

using Map2 = cgogn::CMap2;
using Vec3 = Eigen::Vector3d;

template <typename T>
using VertexAttribute = Map2::VertexAttribute<T>;



static void glfw_error_callback(int error, const char* description)
{
	std::cerr << "Glfw Error "<< error <<" : "<< description << std::endl;
}

class Viewer : public cgogn::rendering_pgl::ImGUIViewer
{
public:
	inline Viewer():
		cgogn::rendering_pgl::ImGUIViewer(1024,1024),
		map_(),
		vertex_position_(),
		vertex_normal_(),
		bb_(),
		render_(nullptr),
		vbo_pos_(nullptr),
//		vbo_norm_(nullptr),
		f(0.0f),
		counter(0)
	{}
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(Viewer);

	void import(const std::string& surface_mesh);

private:
	Map2 map_;
	VertexAttribute<Vec3> vertex_position_;
	VertexAttribute<Vec3> vertex_normal_;
	cgogn::geometry::AABB<Vec3> bb_;
	std::unique_ptr<cgogn::rendering_pgl::MapRender> render_;
	std::unique_ptr<cgogn::rendering_pgl::VBO> vbo_pos_;
//	std::unique_ptr<cgogn::rendering_pgl::VBO> vbo_norm_;
	std::unique_ptr<cgogn::rendering_pgl::ShaderFlat::Param> param_flat_;

public:
	float f;
	int counter;
	cgogn::rendering_pgl::GLColor clear_color;

	void interface() override;
	void draw() override;
	void init() override;
};


void Viewer::interface()
{

	ImGui::Begin("Control Window");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)(clear_color.data())); // Edit 3 floats representing a color
	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		this->counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void Viewer::import(const std::string& surface_mesh)
{
	cgogn::io::import_surface<Vec3>(map_, surface_mesh);

	vertex_position_ = map_.template get_attribute<Vec3, Map2::Vertex>("position");
	if (!vertex_position_.is_valid())
	{
		cgogn_log_error("Viewer::import") << "Missing attribute position. Aborting.";
		std::exit(EXIT_FAILURE);
	}

//	vertex_normal_ = map_.template get_attribute<Vec3, Map2::Vertex>("normal");
//	if (!vertex_normal_.is_valid())
//	{
//		vertex_normal_ = map_.template add_attribute<Vec3, Map2::Vertex>("normal");
//		cgogn::geometry::compute_normal(map_, vertex_position_, vertex_normal_);
//	}

	cgogn::geometry::compute_AABB(vertex_position_, bb_);
	set_scene_radius(cgogn::geometry::diagonal(bb_).norm()/2.0);
	Vec3 center = cgogn::geometry::center(bb_);
	set_scene_center(center);
	show_entire_scene();
}


void Viewer::init()
{
	// create and fill VBO for positions
std::cout<< "0------------" << std::endl;
	vbo_pos_ = cgogn::make_unique<cgogn::rendering_pgl::VBO>(3);
	vbo_pos_->create();
	cgogn::rendering_pgl::update_vbo(vertex_position_, vbo_pos_.get());
std::cout<< "1------------" << std::endl;
	// create and fill VBO for normals
//	vbo_norm_ = cgogn::make_unique<cgogn::rendering_pgl::VBO>(3);
//	cgogn::rendering_pgl::update_vbo(vertex_normal_, vbo_norm_.get());

	// map rendering object (primitive creation & sending to GPU)
	render_ = cgogn::make_unique<cgogn::rendering_pgl::MapRender>();
	std::cout<< "2------------" << std::endl;
	render_->init_primitives(map_, cgogn::rendering_pgl::POINTS);
	render_->init_primitives(map_, cgogn::rendering_pgl::LINES);
	render_->init_primitives(map_, cgogn::rendering_pgl::TRIANGLES, &vertex_position_);
	std::cout<< "3------------" << std::endl;
	param_flat_ = cgogn::rendering_pgl::ShaderFlat::generate_param();
	std::cout<< "4------------" << std::endl;
	param_flat_->set_position_vbo(vbo_pos_.get());
	std::cout<< "5------------" << std::endl;
	param_flat_->front_color_ = cgogn::rendering_pgl::GLColor(0,0.8f,0,1);
	param_flat_->back_color_ = cgogn::rendering_pgl::GLColor(0,0,0.8f,1);
	param_flat_->ambiant_color_ = cgogn::rendering_pgl::GLColor(0.15f,0.15f,0.15f,1);
	std::cout<< "6------------" << std::endl;
}



void Viewer::draw()
{
	glClearColor(clear_color[0],clear_color[1],clear_color[2],clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	cgogn::rendering_pgl::GLMat4 proj = cgogn::rendering_pgl::GLMat4::Identity();
			//get_projection_matrix();
	cgogn::rendering_pgl::GLMat4 view = cgogn::rendering_pgl::GLMat4::Identity();
	//= get_modelview_matrix();

std::cout<< "7------------" << std::endl;
	param_flat_->bind(proj,view);
std::cout<< "8------------" << std::endl;
	render_->draw(cgogn::rendering_pgl::TRIANGLES);
std::cout<< "9------------" << std::endl;
	param_flat_->release();
}


int main(int argc, char** argv)
{
	std::string surface_mesh;
	if (argc < 2)
	{
		cgogn_log_info("simple_viewer") << "USAGE: " << argv[0] << " [filename]";
		surface_mesh = std::string(DEFAULT_MESH_PATH) + std::string("off/aneurysm_3D.off");
		cgogn_log_info("simple_viewer") << "Using default mesh \"" << surface_mesh << "\".";
	}
	else
		surface_mesh = std::string(argv[1]);


	// Instantiate the viewer.
	Viewer view;
//	viewer.setWindowTitle("simple_viewer");
	view.import(surface_mesh);
	// show?
	view.launch("SimpleViewerIMGUI");
	return 0;
}

/*
#include <QApplication>
#include <QMatrix4x4>
#include <QKeyEvent>

#include <QOGLViewer/qoglviewer.h>


	std::unique_ptr<cgogn::rendering_pgl::VBO> vbo_color_;
	std::unique_ptr<cgogn::rendering_pgl::VBO> vbo_sphere_sz_;

	std::unique_ptr<cgogn::rendering_pgl::ShaderBoldLine::Param> param_edge_;
	std::unique_ptr<cgogn::rendering_pgl::ShaderFlat::Param> param_flat_;
	std::unique_ptr<cgogn::rendering_pgl::ShaderVectorPerVertex::Param> param_normal_;
	std::unique_ptr<cgogn::rendering_pgl::ShaderPhongColor::Param> param_phong_;
	std::unique_ptr<cgogn::rendering_pgl::ShaderPointSpriteColorSize::Param> param_point_sprite_;


	std::unique_ptr<cgogn::rendering_pgl::DisplayListDrawer> drawer_;
	std::unique_ptr<cgogn::rendering_pgl::DisplayListDrawer::Renderer> drawer_rend_;

	bool phong_rendering_;
	bool flat_rendering_;
	bool vertices_rendering_;
	bool edge_rendering_;
	bool normal_rendering_;
	bool bb_rendering_;
};


//
// IMPLEMENTATION
//




Viewer::~Viewer()
{}

void Viewer::closeEvent(QCloseEvent*)
{
	render_.reset();
	vbo_pos_.reset();
	vbo_norm_.reset();
	vbo_color_.reset();
	vbo_sphere_sz_.reset();
	drawer_.reset();
	drawer_rend_.reset();
	cgogn::rendering_pgl::ShaderProgram::clean_all();

}

Viewer::Viewer() :

	vbo_color_(nullptr),
	vbo_sphere_sz_(nullptr),
	drawer_(nullptr),
	drawer_rend_(nullptr),
	phong_rendering_(true),
	flat_rendering_(false),
	vertices_rendering_(false),
	edge_rendering_(false),
	normal_rendering_(false),
	bb_rendering_(true)
{}

void Viewer::keyPressEvent(QKeyEvent *ev)
{
	switch (ev->key())
	{
		case Qt::Key_P:
			phong_rendering_ = true;
			flat_rendering_ = false;
			break;
		case Qt::Key_F:
			flat_rendering_ = true;
			phong_rendering_ = false;
			break;
		case Qt::Key_N:
			normal_rendering_ = !normal_rendering_;
			break;
		case Qt::Key_E:
			edge_rendering_ = !edge_rendering_;
			break;
		case Qt::Key_V:
			vertices_rendering_ = !vertices_rendering_;
			break;
		case Qt::Key_B:
			bb_rendering_ = !bb_rendering_;
			break;
		default:
			break;
	}
	// enable QGLViewer keys
	QOGLViewer::keyPressEvent(ev);
	//update drawing
	update();
}

void Viewer::draw()
{
	QMatrix4x4 proj;
	QMatrix4x4 view;
	camera()->getProjectionMatrix(proj);
	camera()->getModelViewMatrix(view);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 2.0f);
	if (flat_rendering_)
	{
		param_flat_->bind(proj,view);
		render_->draw(cgogn::rendering_pgl::TRIANGLES);
		param_flat_->release();
	}

	if (phong_rendering_)
	{
		param_phong_->bind(proj,view);
		render_->draw(cgogn::rendering_pgl::TRIANGLES);
		param_phong_->release();
	}
	glDisable(GL_POLYGON_OFFSET_FILL);

	if (vertices_rendering_)
	{
		param_point_sprite_->bind(proj,view);
		render_->draw(cgogn::rendering_pgl::POINTS);
		param_point_sprite_->release();
	}

	if (edge_rendering_)
	{
		param_edge_->bind(proj,view);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		render_->draw(cgogn::rendering_pgl::LINES);
		glDisable(GL_BLEND);
		param_edge_->release();
	}

	if (normal_rendering_)
	{
		param_normal_->bind(proj,view);
		render_->draw(cgogn::rendering_pgl::POINTS);
		param_normal_->release();
	}

	if (bb_rendering_)
		drawer_rend_->draw(proj,view);
}

void Viewer::init()
{
	glClearColor(0.1f,0.1f,0.3f,0.0f);

	// create and fill VBO for positions
	vbo_pos_ = cgogn::make_unique<cgogn::rendering_pgl::VBO>(3);
	cgogn::rendering_pgl::update_vbo(vertex_position_, vbo_pos_.get());

	// create and fill VBO for normals
	vbo_norm_ = cgogn::make_unique<cgogn::rendering_pgl::VBO>(3);
	cgogn::rendering_pgl::update_vbo(vertex_normal_, vbo_norm_.get());

	// fill a color vbo with abs of normals
	vbo_color_ = cgogn::make_unique<cgogn::rendering_pgl::VBO>(3);
	cgogn::rendering_pgl::update_vbo(vertex_normal_, vbo_color_.get(), [] (const Vec3& n) -> std::array<float,3>
	{
		return {float(std::abs(n[0])), float(std::abs(n[1])), float(std::abs(n[2]))};
	});

	// fill a sphere size vbo
	vbo_sphere_sz_ = cgogn::make_unique<cgogn::rendering_pgl::VBO>(1);
	cgogn::rendering_pgl::update_vbo(vertex_normal_, vbo_sphere_sz_.get(), [&] (const Vec3& n) -> float
	{
		return cgogn::geometry::diagonal(bb_).norm()/1000.0 * (1.0 + 2.0*std::abs(n[2]));
	});

	// map rendering object (primitive creation & sending to GPU)
	render_ = cgogn::make_unique<cgogn::rendering_pgl::MapRender>();
	render_->init_primitives(map_, cgogn::rendering_pgl::POINTS);
	render_->init_primitives(map_, cgogn::rendering_pgl::LINES);
	render_->init_primitives(map_, cgogn::rendering_pgl::TRIANGLES, &vertex_position_);

	// generation of one parameter set (for this shader) : vbo + uniforms
	param_point_sprite_ = cgogn::rendering_pgl::ShaderPointSpriteColorSize::generate_param();
	// set vbo param (see param::set_vbo signature)
	param_point_sprite_->set_all_vbos(vbo_pos_.get(), vbo_color_.get(), vbo_sphere_sz_.get());
	// set uniforms data

	param_edge_ = cgogn::rendering_pgl::ShaderBoldLine::generate_param();
	param_edge_->set_position_vbo(vbo_pos_.get());
	param_edge_->color_ = QColor(255,255,0);
	param_edge_->width_= 2.5f;

	param_flat_ = cgogn::rendering_pgl::ShaderFlat::generate_param();
	param_flat_->set_position_vbo(vbo_pos_.get());
	param_flat_->front_color_ = QColor(0,200,0);
	param_flat_->back_color_ = QColor(0,0,200);
	param_flat_->ambiant_color_ = QColor(5,5,5);

	param_normal_ = cgogn::rendering_pgl::ShaderVectorPerVertex::generate_param();
	param_normal_->set_all_vbos(vbo_pos_.get(), vbo_norm_.get());
	param_normal_->color_ = QColor(200,0,200);
	param_normal_->length_ = cgogn::geometry::diagonal(bb_).norm()/50;

	param_phong_ = cgogn::rendering_pgl::ShaderPhongColor::generate_param();
	param_phong_->set_all_vbos(vbo_pos_.get(), vbo_norm_.get(), vbo_color_.get());

	// drawer for simple old-school g1 rendering
	drawer_ = cgogn::make_unique<cgogn::rendering_pgl::DisplayListDrawer>();
	drawer_rend_= drawer_->generate_renderer();
	drawer_->new_list();
	drawer_->line_width_aa(2.0);
	drawer_->begin(GL_LINE_LOOP);
	drawer_->color3f(1.0,1.0,1.0);
	drawer_->vertex3f(bb_.min()[0],bb_.min()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.min()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.max()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.min()[0],bb_.max()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.min()[0],bb_.max()[1],bb_.max()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.max()[1],bb_.max()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.min()[1],bb_.max()[2]);
	drawer_->vertex3f(bb_.min()[0],bb_.min()[1],bb_.max()[2]);
	drawer_->end();
	drawer_->begin(GL_LINES);
	drawer_->color3f(1.0,1.0,1.0);
	drawer_->vertex3f(bb_.min()[0],bb_.min()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.min()[0],bb_.max()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.min()[0],bb_.min()[1],bb_.max()[2]);
	drawer_->vertex3f(bb_.min()[0],bb_.max()[1],bb_.max()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.min()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.min()[1],bb_.max()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.max()[1],bb_.min()[2]);
	drawer_->vertex3f(bb_.max()[0],bb_.max()[1],bb_.max()[2]);
	drawer_->end();
	drawer_->end_list();
}

int main(int argc, char** argv)
{
	std::string surface_mesh;
	if (argc < 2)
	{
		cgogn_log_info("simple_viewer") << "USAGE: " << argv[0] << " [filename]";
		surface_mesh = std::string(DEFAULT_MESH_PATH) + std::string("off/aneurysm_3D.off");
		cgogn_log_info("simple_viewer") << "Using default mesh \"" << surface_mesh << "\".";
	}
	else
		surface_mesh = std::string(argv[1]);

	QApplication application(argc, argv);
	qoglviewer::init_ogl_context();

	// Instantiate the viewer.
	Viewer viewer;
	viewer.setWindowTitle("simple_viewer");
	viewer.import(surface_mesh);
	viewer.show();

	// Run main loop.
	return application.exec();
}
*/
