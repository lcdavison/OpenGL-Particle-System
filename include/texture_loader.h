#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include <iostream>
#include <cstdio>

#include <glew.h>

#include <png.h>

#include "texture.h"

class TextureLoader 
{
	public:

		TextureLoader ( const std::string& );
		~TextureLoader ( );

		bool		load ( );
		Texture* 	get_texture ( );

	private:

		FILE*		m_file_ptr = nullptr;

		Texture*	m_texture_ptr = nullptr;
		unsigned char*  m_data_ptr = nullptr;
};
#endif
