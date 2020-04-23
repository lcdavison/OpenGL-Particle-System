#ifndef SHADER_H
#define SHADER_H

#include <glew.h>
#include <fstream>
#include <iostream>

class Shader 
{
	public:

		Shader ( );

		bool	compile ( const std::string&, const std::string& );
		void 	begin ( );

		GLuint  get_shader_program ( ) const;

	private:

		GLuint 	load_shader ( const std::string&, GLenum );

		GLuint 	m_shader_program;
};
#endif
