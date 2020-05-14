#include "texture_loader.h"

TextureLoader::TextureLoader ( const std::string& p_filename )
{
	//	Open PNG file for reading
	m_file_ptr = fopen ( p_filename.c_str ( ), "rb" );

	m_texture_ptr = new Texture;
}

TextureLoader::~TextureLoader ( )
{
	fclose ( m_file_ptr );

	delete m_file_ptr;
	m_file_ptr = nullptr;

	delete m_data_ptr;
	m_data_ptr = nullptr;
}

bool TextureLoader::load ( )
{
	//	Check file opened successfully
	if ( !m_file_ptr )
	{
		std::cerr << "Failed to open file" << std::endl;
		return false;
	}

	//	Read signature
	unsigned char signature [ 8 ];
	fread ( signature, 8, 1, m_file_ptr );

	if ( !png_check_sig ( signature, 8 ) )
	{
		std::cerr << "Not PNG file" << std::endl;
		return false;
	}

	//	Setup libpng
	png_structp png_read 	= png_create_read_struct ( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	png_infop png_info 	= png_create_info_struct ( png_read );

	//	Read info chunks
	png_init_io 	  ( png_read, m_file_ptr );
	png_set_sig_bytes ( png_read, 8 );
	png_read_info 	  ( png_read, png_info );

	//	Retrieve image properties
	int bitdepth;
	png_get_IHDR ( png_read, png_info, &m_texture_ptr->m_width, &m_texture_ptr->m_height, &bitdepth, &m_texture_ptr->m_colourtype, NULL, NULL, NULL );

	unsigned int height = m_texture_ptr->m_height;

	//	Setup image memory
	unsigned int bytes_per_row = png_get_rowbytes ( png_read, png_info );
	m_data_ptr = new unsigned char [ bytes_per_row * height ];

	//	Get image rows
	for ( int i = 0; i < height; ++i )
	{
		unsigned char* row_pointer = m_data_ptr + ( i * bytes_per_row );
		png_read_rows ( png_read, &row_pointer, NULL, 1 );
	}

	png_destroy_read_struct ( &png_read, &png_info, NULL );

	return true;
}

Texture* TextureLoader::get_texture ( )
{
	//	Create GL Texture
	GLuint texture_id;
	glCreateTextures ( GL_TEXTURE_2D, 1, &texture_id );

	glBindTexture ( GL_TEXTURE_2D, texture_id );

	//	Set id
	m_texture_ptr->m_texture_id = texture_id;

	//	Determine format
	GLenum format;
	switch ( m_texture_ptr->m_colourtype )
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			format = GL_RGBA;
			break;
		default:
			format = GL_RGB;
			break;
	}

	//	Buffer data
	glTexImage2D ( GL_TEXTURE_2D, 0, format, m_texture_ptr->m_width, m_texture_ptr->m_height, 0, format, GL_UNSIGNED_BYTE, m_data_ptr );

	glGenerateMipmap ( GL_TEXTURE_2D );

	//	Set filter properties
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glBindTexture ( GL_TEXTURE_2D, 0 );

	return m_texture_ptr;
}
