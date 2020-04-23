#include "shader.h"

Shader::Shader ( ) { }

bool Shader::compile ( const std::string& p_vertex_path, const std::string& p_fragment_path )
{
	//	Load and compile shaders
	GLuint vertex_id 	= load_shader ( p_vertex_path, GL_VERTEX_SHADER );
	GLuint fragment_id 	= load_shader ( p_fragment_path, GL_FRAGMENT_SHADER );

	//	Check for failures
	if ( vertex_id < 0 || fragment_id < 0 )		return false;

	//	Create program
	m_shader_program = glCreateProgram ( );

	//	Attach loaded shaders
	glAttachShader ( m_shader_program, vertex_id );
	glAttachShader ( m_shader_program, fragment_id );

	//	Link shaders to shader program
	glLinkProgram  ( m_shader_program );

	//	Check if link was successful
	GLint result;
	glGetProgramiv ( m_shader_program, GL_LINK_STATUS, &result );

	if ( !result )
	{
		std::cerr << "Failed to link shader program" << std::endl;

		GLint log_length;
		glGetProgramiv ( m_shader_program, GL_INFO_LOG_LENGTH, &log_length );

		char log [ log_length ];
		glGetProgramInfoLog ( m_shader_program, log_length, NULL, log );

		std::cout << log << std::endl;

		return false;
	}

	//	Delete shader objects once complete
	glDeleteShader ( vertex_id );
	glDeleteShader ( fragment_id );

	return true;
} 

void Shader::begin ( )
{
	glUseProgram ( m_shader_program );
}

GLuint Shader::load_shader ( const std::string& p_file_path, GLenum p_shader_type )
{
	//	Open shader file
	std::ifstream shader;
	shader.open ( p_file_path, std::ifstream::in );

	if ( !shader.is_open ( ) )
	{
		std::cerr << "Failed to open file " << p_file_path << std::endl;
		return -1;
	}

	//	Get file size
	shader.seekg ( 0, shader.end );
	int size = shader.tellg ( );
	shader.seekg ( 0, shader.beg );

	//	Read file data into buffer
	char* buffer = new char [ size + 1 ];
	shader.read ( buffer, size );
	buffer [ shader.gcount ( ) - 1 ] = '\0';

	shader.close ( );

	//	Setup shader
	GLuint shader_id;
	shader_id = glCreateShader ( p_shader_type );

	//	Set shader source code
	glShaderSource ( shader_id, 1, &buffer, NULL );

	glCompileShader ( shader_id );

	//	Check if compiled successfully
	GLint result;
	glGetShaderiv ( shader_id, GL_COMPILE_STATUS, &result );

	if ( !result )
	{
		std::cerr << "Failed to compile shader" << std::endl;
	
		GLint log_length;
		glGetShaderiv ( shader_id, GL_INFO_LOG_LENGTH, &log_length );

		char log [ log_length ];
		glGetShaderInfoLog ( shader_id, log_length, NULL, log );

		std::cout << log << std::endl;

		delete [ ] buffer;

		return -1;
	}

	std::cout << "Successful" << std::endl;

	return shader_id;
}

GLuint Shader::get_shader_program ( ) const
{
	return m_shader_program;
}
