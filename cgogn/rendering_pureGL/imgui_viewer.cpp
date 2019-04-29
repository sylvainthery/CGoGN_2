
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

#ifndef CGOGN_RENDERING_PURE_GL_VIEWER_H_
#define CGOGN_RENDERING_PURE_GL_VIEWER_H_

//#include <GL/gl3w.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

#include <Eigen/Geometry>
#include <Eigen/SVD>

//#include <cgogn/rendering/cgogn_rendering_export.h>
#include <cgogn/rendering/imgui_viewer.h>

namespace cgogn
{
namespace rendering
{

ImGUIViewer::ImGUIViewer(int32 w, int32 h)
{
	width_ = w;
	height_ = h;
}

void ImGUIViewer::init()
{}

void ImGUIViewer::draw()
{}

void ImGUIViewer::close_event()
{}


void ImGUIViewer::launch()
{
	if (!glfwInit())
		return 1;

	// GL 3.3 + GLSL 130
	const char* glsl_version = "#version 140";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	bool err = gl3wInit() != 0;
	if (err)
	{
		std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();//ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version)
	glfwSetMouseButtonCallback(window, [this](GLFWwindow*, int b, int a, int m)
	{
		switch(a)
		{
			case GLFW_PRESS:
			this->mouse_press_event(b,last_mouse_x_,last_mouse_y_);
			break;
			case GLFW_RELEASE:
			this->key_press_event(k);
			break;
		}
	});

	glfwSetCursorPosCallback(window, [this](GLFWwindow*, double x, double y)
	{
		if (mouse_buttons_)
		{
			mouse_move_event( mouse_buttons_, x,y);
		}
	});

	glfwSetKeyCallback(window, [this](GLFWwindow*, int k, int s, int a, int m)
	{
		shift_pressed_   = (k==GLFW_KEY_LEFT_SHIFT)||(k==GLFW_KEY_RIGHT_SHIFT)
		control_pressed_ = (k==GLFW_KEY_LEFT_CONTROL)||(k==GLFW_KEY_RIGHT_CONTROL)
		alt_pressed_     = (k==GLFW_KEY_LEFT_ALT)||(k==GLFW_KEY_RIGHT_ALT);
		meta_pressed_    = (k==GLFW_KEY_LEFT_SUPER)||(k==GLFW_KEY_RIGHT_SUPER)
		switch(a)
		{
			case GLFW_PRESS:
			if (k==GLFW_KEY_ESCAPE)
				exit(0);
			this->key_press_event(k);
			break;
			case GLFW_RELEASE:
			this->key_press_event(k);
			break;
		}
	});

	while (!glfwWindowShouldClose(window))
	{
		if (animated)
		{
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Render();
			draw_interface();
			int display_w, display_h;
			glfwMakeContextCurrent(window);
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(vp_x_,vp_y_, width_, height_);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			draw();
			glfwMakeContextCurrent(window);
			glfwSwapBuffers(window);
		}
	}
}

}
}
