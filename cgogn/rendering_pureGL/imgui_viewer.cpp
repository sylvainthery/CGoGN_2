
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
#include <iostream>
#include <cgogn/rendering_pureGL/imgui_viewer.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace cgogn
{
namespace rendering_pgl
{

ImGUIViewer::ImGUIViewer()
{
	width_ = 512;
	height_ = 512;
	need_draw_ = true;
}

ImGUIViewer::ImGUIViewer(int32 w, int32 h)
{
	width_ = w;
	height_ = h;
	need_draw_ = true;
}

ImGUIViewer::~ImGUIViewer()
{}

void ImGUIViewer::init()
{}

void ImGUIViewer::draw()
{}

void ImGUIViewer::close_event()
{}

void ImGUIViewer::interface()
{}

void ImGUIViewer::mouse_press_event(int32 buttons, float64 x, float64 y)
{}

void ImGUIViewer::mouse_release_event(int32 buttons, float64 x, float64 y)
{}

void ImGUIViewer::mouse_move_event(int32 buttons, float64 x, float64 y)
{}

void ImGUIViewer::mouse_dbl_click_event(int32 buttons, float64 x, float64 y)
{}

void ImGUIViewer::mouse_wheel_event(float64 x, float64 y)
{}

void ImGUIViewer::key_press_event(int32 key_code)
{}

void ImGUIViewer::key_release_event(int32 key_code)
{}


bool ImGUIViewer::launch(const std::string& name)
{
	if (!glfwInit())
		return false;

	// GL 3.3 + GLSL 130
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width_, height_, name.c_str(), NULL, NULL);
	if (window == NULL)
		return false;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	bool err = gl3wInit() != 0;
	if (err)
	{
		std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
		return false;
	}

	init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();//ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	glfwSetWindowUserPointer(window,this);
	glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int b, int a, int m)
	{
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		switch(a)
		{
			case GLFW_PRESS:
			that->mouse_press_event(b,that->last_mouse_x_,that->last_mouse_y_);
			break;
			case GLFW_RELEASE:
			that->mouse_release_event(b,that->last_mouse_x_,that->last_mouse_y_);
			break;
		}
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y)
	{
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		if (that->mouse_buttons_)
		{
			that->mouse_move_event( that->mouse_buttons_, x,y);
		}
	});

	glfwSetKeyCallback(window, [](GLFWwindow* w, int k, int s, int a, int m)
	{
		ImGUIViewer* that= static_cast<ImGUIViewer*>(glfwGetWindowUserPointer(w));
		that->shift_pressed_   = (k==GLFW_KEY_LEFT_SHIFT)||(k==GLFW_KEY_RIGHT_SHIFT);
		that->control_pressed_ = (k==GLFW_KEY_LEFT_CONTROL)||(k==GLFW_KEY_RIGHT_CONTROL);
		that->alt_pressed_     = (k==GLFW_KEY_LEFT_ALT)||(k==GLFW_KEY_RIGHT_ALT);
		that->meta_pressed_    = (k==GLFW_KEY_LEFT_SUPER)||(k==GLFW_KEY_RIGHT_SUPER);
		switch(a)
		{
			case GLFW_PRESS:
			if (k==GLFW_KEY_ESCAPE)
				exit(0);
			that->key_press_event(k);
			break;
			case GLFW_RELEASE:
			that->key_press_event(k);
			break;
		}
	});

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		interface();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(vp_x_,vp_y_, width_, height_);
		if (need_draw_)
			draw();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	return true;
}

}
}
