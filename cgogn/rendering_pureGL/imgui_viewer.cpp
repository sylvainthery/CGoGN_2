
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


namespace cgogn
{
namespace rendering_pgl
{

ImGUIViewer::ImGUIViewer():
	window(nullptr),
	win_name_("CGoGN")
{
	vp_w_ = 512;
	vp_h_ = 512;
}

ImGUIViewer::ImGUIViewer(int32 w, int32 h)
{
	vp_w_ = w;
	vp_h_ = h;
}

void ImGUIViewer::set_window_title(const std::string&  name)
{
	win_name_ = name;
	if (window)
		glfwSetWindowTitle(window,win_name_.c_str());
}

ImGUIViewer::~ImGUIViewer()
{}

void ImGUIViewer::close_event()
{}

void ImGUIViewer::interface()
{}

void ImGUIViewer::resize_event(int32 w, int32 h)
{}


static void glfw_error_callback(int error, const char* description)
{
	std::cerr <<"Glfw Error "<<error << ": " << description << std::endl;
}


bool ImGUIViewer::launch()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return false;

	// GL 3.3 + GLSL 150 + Core Profile
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	window = glfwCreateWindow(vp_w_, vp_h_, win_name_.c_str(), nullptr, nullptr);
	if (window == nullptr)
		return false;

	glfwMakeContextCurrent(window);

//	bool err = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	bool err = gl3wInit() != 0;
	if (err)
	{
		std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
		return false;
	}
	glfwSwapInterval(1); // Enable vsync

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();//ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	glfwSetWindowUserPointer(window,this);

	std::cout << glGetString(GL_RENDERER)<< std::endl;
	std::cout << glGetString(GL_VERSION)<< std::endl;

	init();

	glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int b, int a, int m)
	{
		if (ImGui::GetIO().WantCaptureMouse) return;
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		glfwGetCursorPos(that->window,&(that->last_mouse_x_),&(that->last_mouse_y_));

		switch(a)
		{
			case GLFW_PRESS:
			that->mouse_buttons_ |= 1<<b;
			that->mouse_press_event(b,that->last_mouse_x_,that->last_mouse_y_);
			break;
			case GLFW_RELEASE:
			that->mouse_buttons_ &= ~(1<<b);
			that->mouse_release_event(b,that->last_mouse_x_,that->last_mouse_y_);
			break;
		}
	});

	glfwSetScrollCallback(window, [](GLFWwindow* w, double dx, double dy)
	{
		if (ImGui::GetIO().WantCaptureMouse) return;
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		that->mouse_wheel_event(dx,100*dy);
	});


	glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y)
	{
		if (ImGui::GetIO().WantCaptureMouse) return;
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		if (that->mouse_buttons_)
		{
			that->mouse_move_event(x,y);
		}
	});

	glfwSetKeyCallback(window, [](GLFWwindow* w, int k, int s, int a, int m)
	{
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		that->shift_pressed_   = (m & GLFW_MOD_SHIFT);
		that->control_pressed_ = (m & GLFW_MOD_CONTROL);
		that->alt_pressed_     = (m & GLFW_MOD_ALT);
		that->meta_pressed_    = (m & GLFW_MOD_SUPER);;
		switch(a)
		{
			case GLFW_PRESS:
			if (k==GLFW_KEY_ESCAPE)
				exit(0);
			if ((k==GLFW_KEY_F) && that->control_pressed_  && !that->shift_pressed_)
			{
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(that->window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				glfwSetInputMode(that->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				return;
			}
			if ((k==GLFW_KEY_F) && that->control_pressed_ && that->shift_pressed_)
			{
				int count;
				GLFWmonitor** monitors = glfwGetMonitors(&count);
				if (count>1)
				{
					const GLFWvidmode* mode = glfwGetVideoMode(monitors[1]);
					glfwSetWindowMonitor(that->window, monitors[1], 0, 0, mode->width, mode->height, mode->refreshRate);
					glfwSetInputMode(that->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				else
					std::cerr << "Only one monitor"<< std::endl;
				return;
			}
			if ((k==GLFW_KEY_W) && that->control_pressed_)
			{
				glfwSetWindowMonitor(that->window, nullptr,100,100, 1024, 1024, 0);
				glfwSetInputMode(that->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				return;
			}
			that->key_press_event(k);
			break;
			case GLFW_RELEASE:
			that->key_release_event(k);
			break;
		}
	});

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (show_imgui_)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			interface();
			ImGui::Render();
		}
		glfwMakeContextCurrent(window);
		int32 nw,nh;
		glfwGetFramebufferSize(window, &nw, &nh);
		if ((nw!=vp_w_)||(nh!=vp_h_))
		{
			vp_w_ = nw;
			vp_h_ = nh;
			resize_event(nw,nh);
			need_redraw_ = true;
		}
		glfwMakeContextCurrent(window);
		cam_.set_aspect_ratio(double(vp_w_)/vp_h_);
		glViewport(0,0, vp_w_, vp_h_);
		spin();
		draw();
		if (show_imgui_)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}
	return true;
}

}
}
