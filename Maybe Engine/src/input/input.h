#pragma once

#include <GLFW\glfw3.h>

#include "..\graphics\graphics.h"

namespace mb { namespace input {

	enum Key 
	{
		Q = GLFW_KEY_Q,
		W = GLFW_KEY_W,
		E = GLFW_KEY_E,
		R = GLFW_KEY_R,
		T = GLFW_KEY_T,
		Y = GLFW_KEY_Y,
		U = GLFW_KEY_U,
		I = GLFW_KEY_I,
		O = GLFW_KEY_O,
		P = GLFW_KEY_P,
		A = GLFW_KEY_A,
		S = GLFW_KEY_S,
		D = GLFW_KEY_D,
		F = GLFW_KEY_F,
		G = GLFW_KEY_G,
		H = GLFW_KEY_H,
		J = GLFW_KEY_J,
		K = GLFW_KEY_K,
		L = GLFW_KEY_L,
		Z = GLFW_KEY_Z,
		X = GLFW_KEY_X,
		C = GLFW_KEY_C,
		V = GLFW_KEY_V,
		B = GLFW_KEY_B,
		N = GLFW_KEY_N,
		M = GLFW_KEY_M
	};

	bool IsKeyPressed(graphics::Window& window, Key key)
	{
		if (glfwGetKey((GLFWwindow*)window.GetGLFWwindow(), key) == GLFW_PRESS)
			return true;
		else
			return false;
	}

	bool IsKeyDown(graphics::Window& window, Key key)
	{

	}

	bool IsKeyUp(graphics::Window& window, Key key)
	{

	}

} }