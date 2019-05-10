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


#include <cgogn/core/utils/definitions.h>
#include <cgogn/rendering_pureGL/imgui_viewer.h>
#include <cgogn/rendering_pureGL/drawer.h>
#include <cgogn/rendering_pureGL/wall_paper.h>
#include <cgogn/rendering_pureGL/types.h>

#define DEFAULT_MESH_PATH CGOGN_STR(CGOGN_TEST_MESHES_PATH)

using Vec3 = Eigen::Vector3d;
namespace RGL = cgogn::rendering_pgl;

class Drawing : public RGL::ImGUIViewer
{
public:
	Drawing();
//	Drawing(Drawing* ptr);
	CGOGN_NOT_COPYABLE_NOR_MOVABLE(Drawing);

	void draw() override;
	bool init() override;
	void interface() override;
	void close_event() override;
	void key_press_event(int k) override;
	virtual ~Drawing() override;

////private:
	std::shared_ptr<RGL::DisplayListDrawer> drawer_;
	std::shared_ptr<RGL::DisplayListDrawer> drawer2_;
	std::unique_ptr<RGL::DisplayListDrawer::Renderer> drawer_rend_;
	std::unique_ptr<RGL::DisplayListDrawer::Renderer> drawer2_rend_;

	std::shared_ptr<RGL::WallPaper> wp_;
	std::shared_ptr<RGL::WallPaper> button_;
	std::unique_ptr<RGL::WallPaper::Renderer> wp_rend_;
	std::unique_ptr<RGL::WallPaper::Renderer> button_rend_;

//	Drawing* m_first;
	float interface_scaling_;

};



Drawing::~Drawing()
{}

void Drawing::close_event()
{
	drawer_rend_.reset();
	drawer2_rend_.reset();
	wp_rend_.reset();
	button_rend_.reset();

	drawer_.reset();
	drawer2_.reset();
	wp_.reset();
	button_.reset();
	RGL::ShaderProgram::clean_all();
}

Drawing::Drawing() :
	drawer_(nullptr),
	drawer2_(nullptr),
	drawer_rend_(nullptr),
	drawer2_rend_(nullptr),
	wp_(nullptr),
	button_(nullptr),
	wp_rend_(nullptr),
	button_rend_(nullptr),
	interface_scaling_(1.0)
{
}



//Drawing::Drawing(Drawing* ptr) :
////	RGL::ImGUIViewer(ptr),
//	drawer_(nullptr),
//	drawer2_(nullptr),
//	drawer_rend_(nullptr),
//	drawer2_rend_(nullptr),
//	wp_(nullptr),
//	button_(nullptr),
//	wp_rend_(nullptr),
//	button_rend_(nullptr),
//	m_first(ptr)
//{}


void Drawing::key_press_event(int k)
{
	std::cout << "key_press_event "<< k << std::endl;
	switch(k)
	{
		case int('S'):
			if (shift_pressed_)
				interface_scaling_ += 0.1f;
			else
				interface_scaling_ -= 0.1f;
			break;
		case int(' '):
			show_imgui_ = !show_imgui_;
			break;
		default:
			break;
	}
}


void Drawing::draw()
{
	wp_rend_->draw();
	button_rend_->draw();

	cgogn::rendering_pgl::GLMat4 proj = get_projection_matrix();
	cgogn::rendering_pgl::GLMat4 view = get_modelview_matrix();

	drawer_rend_->draw(proj,view);
	drawer2_rend_->draw(proj,view);
}

bool Drawing::init()
{
	set_scene_radius(5.0);
	set_scene_center(Eigen::Vector3d(0,0,0));
	glClearColor(0.1f,0.1f,0.3f,0.0f);

	wp_ = std::make_shared<RGL::WallPaper>(RGL::GLImage(std::string(DEFAULT_MESH_PATH) + std::string(("../images/cgogn2.png"))));
	button_ = std::make_shared<RGL::WallPaper>(RGL::GLImage(std::string((DEFAULT_MESH_PATH) + std::string(("../images/igg.png")))));
	button_->set_local_position(0.1f,0.1f,0.2f,0.2f);
	wp_rend_ = wp_->generate_renderer();
	button_rend_ = button_->generate_renderer();

	// drawer for simple old-school g1 rendering
	drawer_ = std::make_shared<RGL::DisplayListDrawer>();
	drawer_rend_ = drawer_->generate_renderer();
	drawer_->new_list();
	drawer_->line_width(2.0);
	drawer_->begin(GL_LINE_LOOP);
		drawer_->color3f(1.0,0.0,0.0);
		drawer_->vertex3f(0.0,0.0,0.0);
		drawer_->color3f(0.0,1.0,1.0);
		drawer_->vertex3f(1,0,0);
		drawer_->color3f(1.0,0.0,1.0);
		drawer_->vertex3f(1.0f,1.0f,0.0f);
		drawer_->color3f(1.0,1.0,0.0);
		drawer_->vertex3f(0,1,0);
	drawer_->end();
	drawer_->line_width_aa(3.0);
	drawer_->begin(GL_LINES);
		drawer_->color3f(0.0,0.8,0.0);
		drawer_->vertex3fv(Vec3(-1,2,0));
		drawer_->color3f(0.0,0.0,0.8);
		drawer_->vertex3fv(Vec3(-1.3,0,0));
		drawer_->color3f(0.0,0.0,0.8);
		drawer_->vertex3fv(Vec3(-2,1,0));
		drawer_->color3f(0.8,0.0,0.0);
		drawer_->vertex3fv(Vec3(-2.3,3,0));
	drawer_->end();

	drawer_->begin(GL_TRIANGLES);
		drawer_->color3f(1.0,0.0,0.0);
		drawer_->vertex3fv({{2,2,0}});
		drawer_->color3f(0.0,1.0,0.0);
		drawer_->vertex3fv({{4,3,0}});
		drawer_->color3f(0.0,0.0,1.0);
		drawer_->vertex3fv({{2.5,1,0}});
	drawer_->end();

	drawer_->point_size_aa(7.0);
	drawer_->begin(GL_POINTS);
	for (float a=0.0f; a < 1.0f; a+= 0.1f)
	{
		Vec3 P(4.0+std::cos(6.28*a),-2.0+std::sin(6.28*a),0.0);
		Vec3 C(a,0.5,1.0-a);
		drawer_->color3fv(C);
		drawer_->vertex3fv(P);
	}
	drawer_->end();

	drawer_->ball_size(0.1f);
	drawer_->begin(GL_POINTS);
	for (float a=0.05f; a < 1.0f; a+= 0.1f)
	{
		Vec3 P(4.0+std::cos(6.28*a)*1.2,-2.0+ std::sin(6.28*a)*1.2, std::sin(6.28*a)*0.2 );
		Vec3 C(a,0.5,1.0-a);
		drawer_->color3fv(C);
		drawer_->vertex3fv(P);
	}

	drawer_->end();
	drawer_->end_list();

	drawer2_ = std::make_shared<RGL::DisplayListDrawer>();
	drawer2_rend_ = drawer2_->generate_renderer();
	drawer2_->new_list();
	drawer2_->point_size_aa(5.0);
	drawer2_->begin(GL_POINTS);
	drawer2_->color3f(1.0,1.0,1.0);
	for (float z=-1.0f; z < 1.0f; z+= 0.1f)
		for (float y=-2.0f; y < 0.0f; y+= 0.1f)
			for (float x=0.0f; x < 2.0f; x+= 0.1f)
			{
				drawer2_->vertex3f(x,y,z);
			}
	drawer2_->end();

	drawer2_->ball_size(0.03f);
	drawer2_->begin(GL_POINTS);
	drawer2_->color3f(1.0,1.0,1.0);
	for (float z=-1.0f; z < 1.0f; z+= 0.2f)
		for (float y=-2.0f; y < 0.0f; y+= 0.2f)
			for (float x=-3.0f; x < -1.0f; x+= 0.2f)
			{
				drawer2_->vertex3f(x,y,z);
			}
	drawer2_->end();
	drawer2_->end_list();
	return true;
}

void Drawing::interface()
{
	ImGui::GetIO().FontGlobalScale = interface_scaling_;

	ImGui::Begin("Control Window",nullptr, ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowSize({0,0});

	ImGui::Separator();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}


int main(int argc, char** argv)
{
	// Instantiate the viewer.

	std::unique_ptr<Drawing> viewer = cgogn::make_unique<Drawing>();
	viewer->set_window_title("Drawing");
	viewer->launch();

	return 0;
}
