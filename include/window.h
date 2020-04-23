#ifndef WINDOW_H
#define WINDOW_H

#include <glew.h>

#define GLFW_DLL
#include <glfw3.h>

#include <iostream>

class Window 
{
	public:

		Window ( );
		Window ( unsigned int, unsigned int );

		~Window ( );

		unsigned int 	get_width ( );
		unsigned int 	get_height ( );
		GLFWwindow* 	get_glfw_ptr ( );

	private:

		unsigned int 	m_width;
		unsigned int 	m_height;

		GLFWwindow* 	m_window_ptr = nullptr;
};
#endif
