
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

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cgogn/rendering_pureGL/imgui_viewer.h>
#include <cgogn/rendering_pureGL/fbo.h>


namespace cgogn
{
namespace rendering_pgl
{

static void glfw_error_callback(int error, const char* description)
{
	std::cerr <<"Glfw Error "<<error << ": " << description << std::endl;
}


ImGUIViewer::ImGUIViewer(ImGUIViewer* share):
	vp_percent_x_(0),
	vp_percent_y_(0),
	vp_percent_width_(1),
	vp_percent_height_(1),
	double_click_timeout_(0.2),
	last_click_time_(0),
	global_fbo_(nullptr)
{
	bool err = gl3wInit();
}


void ImGUIViewer::set_viewer_geometry(float64 px, float64 py, float64 pw, float64 ph, float64 frame_width, float64 frame_height)
{
	fr_w_ = frame_width;
	fr_h_ = frame_height;
	vp_percent_x_ = px;
	vp_percent_y_ = py;
	vp_percent_width_ = pw;
	vp_percent_height_ = ph;
	vp_x_ = int32(vp_percent_x_*frame_width);
	vp_y_ = int32(vp_percent_y_*frame_height);
	vp_w_ = int32(vp_percent_width_*frame_width);
	vp_h_ = int32(vp_percent_height_*frame_height);
	resize_event(vp_w_,vp_h_);
}

void ImGUIViewer::update_viewer_geometry(float64 frame_width, float64 frame_height)
{
	fr_w_ = frame_width;
	fr_h_ = frame_height;
	vp_x_ = int32(vp_percent_x_*frame_width);
	vp_y_ = int32(vp_percent_y_*frame_height);
	vp_w_ = int32(vp_percent_width_*frame_width);
	vp_h_ = int32(vp_percent_height_*frame_height);
	resize_event(vp_w_,vp_h_);
}


ImGUIViewer::~ImGUIViewer()
{}

void ImGUIViewer::resize_event(int32 , int32 )
{
}


void ImGUIViewer::key_press_event(int32 key_code)
{}

void ImGUIViewer::key_release_event(int32 key_code)
{}

void ImGUIViewer::close_event()
{}

bool ImGUIViewer::get_pixel_scene_position(int32 x, int32 y, GLVec3d& P)
{
	float z;
	GLint xs,ys;
	float64 xogl;
	float64 yogl;
	float64 zogl;
	if (global_fbo_ != nullptr)
	{
		xs = GLint(double(x-vp_x_) / double(vp_w_) * global_fbo_->width());
		ys = GLint(double((fr_h_-y)-vp_y_) / double(vp_h_) * global_fbo_->height());
		global_fbo_->bind();
		glReadBuffer(GL_DEPTH_ATTACHMENT);
		glReadPixels(xs, ys,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&z);
		global_fbo_->release();
		if (z>=1.0f)
			return false;
		xogl = (float64(xs)/vp_w_)*2.0 - 1.0;
		yogl = (float64(ys)/vp_h_)*2.0 - 1.0;
		zogl = float64(z)*2.0 - 1.0;
	}
	else
	{
		xs = x;
		ys = fr_h_-y;
		glReadBuffer(GL_DEPTH_ATTACHMENT);
		glReadPixels(xs,ys,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&z);
		if (z>=1.0f)
			return false;
		float64 xogl = (float64(xs-vp_x_)/vp_w_)*2.0 - 1.0;
		float64 yogl = (float64(ys-vp_y_)/vp_h_)*2.0 - 1.0;
		float64 zogl = float64(z)*2.0 - 1.0;
	}

	GLVec4d Q(xogl, yogl, zogl, 1.0);
	GLMat4d m = camera().get_projection_matrix_d()*camera().get_modelview_matrix_d();
	GLMat4d im = m.inverse();

	GLVec4d P4 = im * Q;
	if (Q.w() != 0.0)
	{
		P.x() = P4.x() / P4.w();
		P.y() = P4.y() / P4.w();
		P.z() = P4.z() / P4.w();
		return true;
	}
	return false;

}

ImGUIApps::ImGUIApps():
	window_(nullptr),
	win_name_("CGoGN"),
	win_frame_width_(512),
	win_frame_height_(512),
	show_imgui_(true)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GFLW!" << std::endl;
	}

	// GL 3.3 + GLSL 150 + Core Profile
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	window_ = glfwCreateWindow(win_frame_width_, win_frame_height_, win_name_.c_str(), nullptr, nullptr);
	if (window_ == nullptr)
	{
		std::cerr << "Failed to create Window!" << std::endl;
	}

	glfwMakeContextCurrent(window_);

	bool err = gl3wInit() != 0;
	if (err)
	{
		std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
	}
	glfwSwapInterval(1); // Enable vsync

	IMGUI_CHECKVERSION();
	context_ = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();//ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	glfwSetWindowUserPointer(window_,this);

	std::cout << glGetString(GL_VENDOR)<< std::endl;
	std::cout << glGetString(GL_RENDERER)<< std::endl;
	std::cout << glGetString(GL_VERSION)<< std::endl;

	glfwSetWindowSizeCallback(window_, [](GLFWwindow* wi, int , int)
	{
		ImGUIApps* that = static_cast<ImGUIApps*>(glfwGetWindowUserPointer(wi));
		glfwGetFramebufferSize(wi, &(that->win_frame_width_), &(that->win_frame_height_));

		for (ImGUIViewer* v: that->viewers_)
		{
			v->update_viewer_geometry(that->win_frame_width_,that->win_frame_height_);
			v->cam_.set_aspect_ratio(double(v->vp_w_)/v->vp_h_);
			v->need_redraw_ = true;
			v->resize_event(v->vp_w_,v->vp_h_);
		}
	});

	glfwSetMouseButtonCallback(window_, [](GLFWwindow* wi, int b, int a, int m)
	{
		if (ImGui::GetIO().WantCaptureMouse) return;
		ImGUIApps* that = static_cast<ImGUIApps*>(glfwGetWindowUserPointer(wi));
		double cx,cy;
		glfwGetCursorPos(wi,&cx,&(cy));
		for (ImGUIViewer* v: that->viewers_)
		{
			if (v->over_viewport(cx,cy))
			{
				v->last_mouse_x_ = cx;
				v->last_mouse_y_ = cy;
				double now = glfwGetTime();
				switch(a)
				{
					case GLFW_PRESS:
					v->mouse_buttons_ |= 1<<b;
					v->mouse_press_event(b,v->last_mouse_x_,v->last_mouse_y_);
					if (now - v->last_click_time_ < v->double_click_timeout_)
						v->mouse_dbl_click_event(b,v->last_mouse_x_,v->last_mouse_y_);
					v->last_click_time_ = now;
					break;
					case GLFW_RELEASE:
					v->mouse_buttons_ &= ~(1<<b);
					v->mouse_release_event(b,v->last_mouse_x_,v->last_mouse_y_);
					break;
				}
			}
		}
	});

	glfwSetScrollCallback(window_, [](GLFWwindow* wi, double dx, double dy)
	{
		if (ImGui::GetIO().WantCaptureMouse) return;
		double cx,cy;
		glfwGetCursorPos(wi,&cx,&(cy));
		ImGUIApps* that = static_cast<ImGUIApps*>(glfwGetWindowUserPointer(wi));
		for (ImGUIViewer* v: that->viewers_)
		{
			if (v->over_viewport(cx,cy))
				v->mouse_wheel_event(dx,100*dy);
		}
	});


	glfwSetCursorPosCallback(window_, [](GLFWwindow* wi, double x, double y)
	{
		if (ImGui::GetIO().WantCaptureMouse) return;
		double cx,cy;
		glfwGetCursorPos(wi,&cx,&(cy));
		ImGUIApps* that = static_cast<ImGUIApps*>(glfwGetWindowUserPointer(wi));
		for (ImGUIViewer* v: that->viewers_)
		{
			if (v->mouse_buttons_ && v->over_viewport(cx,cy))
				v->mouse_move_event(x,y);
		}
	});

	glfwSetKeyCallback(window_, [](GLFWwindow* wi, int k, int s, int a, int m)
	{
		double cx,cy;
		glfwGetCursorPos(wi,&cx,&(cy));
		ImGUIApps* that = static_cast<ImGUIApps*>(glfwGetWindowUserPointer(wi));
		for (ImGUIViewer* v: that->viewers_)
		{
			if ( v->over_viewport(cx,cy))
			{
				v->shift_pressed_   = (m & GLFW_MOD_SHIFT);
				v->control_pressed_ = (m & GLFW_MOD_CONTROL);
				v->alt_pressed_     = (m & GLFW_MOD_ALT);
				v->meta_pressed_    = (m & GLFW_MOD_SUPER);;
				switch(a)
				{
					case GLFW_PRESS:
					if (k==GLFW_KEY_ESCAPE)
						exit(0);
					if ((k==GLFW_KEY_F) && v->control_pressed_  && !v->shift_pressed_)
					{
						GLFWmonitor* monitor = glfwGetPrimaryMonitor();
						const GLFWvidmode* mode = glfwGetVideoMode(monitor);
						glfwSetWindowMonitor(wi, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
						glfwSetInputMode(wi, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
						return;
					}
					if ((k==GLFW_KEY_F) && v->control_pressed_ && v->shift_pressed_)
					{
						int count;
						GLFWmonitor** monitors = glfwGetMonitors(&count);
						if (count>1)
						{
							const GLFWvidmode* mode = glfwGetVideoMode(monitors[1]);
							glfwSetWindowMonitor(wi, monitors[1], 0, 0, mode->width, mode->height, mode->refreshRate);
							glfwSetInputMode(wi, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
						}
						else
							std::cerr << "Only one monitor"<< std::endl;
						return;
					}
					if ((k==GLFW_KEY_W) && v->control_pressed_)
					{
						glfwSetWindowMonitor(wi, nullptr,100,100, 1024, 1024, 0);
						glfwSetInputMode(wi, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
						return;
					}
					v->key_press_event(k);
					break;
					case GLFW_RELEASE:
					v->key_release_event(k);
					break;
				}
			}
		}
	});

}

ImGUIApps::~ImGUIApps()
{

}


void ImGUIApps::close_event()
{
	for (ImGUIViewer* v: viewers_)
	{
		v->close_event();
	}
}


void ImGUIApps::interface()
{}


void ImGUIApps::set_window_size(int32 w, int32 h)
{
	glfwSetWindowSize(window_,w,h);
}

void ImGUIApps::set_window_title(const std::string&  name)
{
	win_name_ = name;
	if (window_)
		glfwSetWindowTitle(window_,win_name_.c_str());
}


void ImGUIApps::add_view(ImGUIViewer* view)
{
	glfwMakeContextCurrent(window_);
	view->init();
	view->mouse_buttons_ = 0;
	viewers_.push_back(view);
}

void ImGUIApps::adapt_viewers_geometry()
{
	switch(viewers_.size())
	{
		case 1:
			viewers_[0]->set_viewer_geometry(0,0,1,1,win_frame_width_,win_frame_height_);
			break;
		case 2:
			viewers_[0]->set_viewer_geometry(0,0,0.5,1,win_frame_width_,win_frame_height_);
			viewers_[1]->set_viewer_geometry(0.5,0,0.5,1,win_frame_width_,win_frame_height_);
			break;
		case 3:
			viewers_[0]->set_viewer_geometry(0,0,0.5,0.5,win_frame_width_,win_frame_height_);
			viewers_[1]->set_viewer_geometry(0.5,0,0.5,0.5,win_frame_width_,win_frame_height_);
			viewers_[2]->set_viewer_geometry(0,0,1,0.5,win_frame_width_,win_frame_height_);
			break;
		case 4:
			viewers_[0]->set_viewer_geometry(0,0,0.5,0.5,win_frame_width_,win_frame_height_);
			viewers_[1]->set_viewer_geometry(0.5,0,0.5,0.5,win_frame_width_,win_frame_height_);
			viewers_[2]->set_viewer_geometry(0,0.5,0.5,0.5,win_frame_width_,win_frame_height_);
			viewers_[3]->set_viewer_geometry(0.5,0.5,0.5,0.5,win_frame_width_,win_frame_height_);
			break;
	}
}

void ImGUIApps::launch()
{
	adapt_viewers_geometry();

	for (ImGUIViewer* v: viewers_)
		v->need_redraw_ = true;

	while (!glfwWindowShouldClose(window_))
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		glfwMakeContextCurrent(window_);
		if (show_imgui_)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			interface();
			ImGui::Render();
		}
		for(ImGUIViewer* v: viewers_)
		{
			v->spin();
			v->set_vp();
			v->draw();
		}
		if (show_imgui_)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window_);
	}
	glfwDestroyWindow(window_);
}

}
}
