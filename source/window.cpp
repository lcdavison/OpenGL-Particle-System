#include "window.h"

Window::Window ( ) { }

Window::Window ( unsigned int p_width, unsigned int p_height ) : m_width ( p_width ), m_height ( p_height )
{
	glfwInit ( );

	glfwWindowHint ( GLFW_DOUBLEBUFFER, GLFW_TRUE );
	glfwWindowHint ( GLFW_VISIBLE, GLFW_TRUE );
	glfwWindowHint ( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 0 );

	m_window_ptr = glfwCreateWindow ( m_width, m_height, "Particle System - Luke Davison", NULL, NULL );

	glfwMakeContextCurrent ( m_window_ptr );

	if ( glewInit ( ) != GLEW_OK )
	{
		std::cout << "Failed to initialise GLEW" << std::endl;
	}

	glfwShowWindow ( m_window_ptr );
}

Window::~Window ( )
{
	glfwDestroyWindow ( m_window_ptr );
	m_window_ptr = nullptr;
}

unsigned int Window::get_width ( )
{
	return m_width;
}

unsigned int Window::get_height ( )
{
	return m_height;
}

GLFWwindow* Window::get_glfw_ptr ( )
{
	return m_window_ptr;
}
