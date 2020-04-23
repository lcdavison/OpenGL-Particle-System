#include "particle_system.h"

#include <iostream>
#include <cstring>

static glm::vec3 positions [ ] = 
{
	glm::vec3 ( -0.5f, -0.5f, 0.0f ),
	glm::vec3 ( 0.5f, -0.5f, 0.0f ),
	glm::vec3 ( -0.5f, 0.5f, 0.0f ),
	glm::vec3 ( 0.5f, 0.5f, 0.0f )
};

static glm::vec2 tex_coords [ ] = 
{
	glm::vec2 ( 0.0f, 0.0f ),
	glm::vec2 ( 1.0f, 0.0f ),
	glm::vec2 ( 0.0f, 1.0f ),
	glm::vec2 ( 1.0f, 1.0f )
};

ParticleSystem::ParticleSystem ( ) : m_range ( glm::vec2 ( -1.0f, 1.0f ) )  { }

ParticleSystem::ParticleSystem ( const glm::vec3& p_position, const glm::vec3& p_colour, unsigned int p_num_particles ) : 
	m_position ( p_position ), 
	m_colour ( p_colour ), 
	m_num_particles ( p_num_particles ), 
	m_particles_per_frame ( 1 ), 
	m_range ( glm::vec2 ( -1.0f, 1.0f ) )
{
	m_particles.reserve ( p_num_particles );
}

void ParticleSystem::setup_buffers ( )
{
	//	Setup colour data
	std::vector < glm::vec3 > colours;
	colours.resize ( 4, m_colour );

	//	Create Vertex Array
	glCreateVertexArrays ( 1, &m_vertex_array );
	glBindVertexArray ( m_vertex_array );

	//	Create vertex buffer
	glCreateBuffers ( 1, &m_vertex_buffer );
	glBindBuffer ( GL_ARRAY_BUFFER, m_vertex_buffer );

	//	Allocate buffer memory
	glBufferData ( GL_ARRAY_BUFFER, sizeof ( positions ) + sizeof ( tex_coords ) + ( 4 * sizeof ( glm::vec3 ) ), NULL, GL_STATIC_DRAW );

	/*	
	 *	Quad Data
	 * */	
	
	//	Vertices
	glBufferSubData ( GL_ARRAY_BUFFER, 0, sizeof ( positions ), &positions [ 0 ] );
	glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	//	Colours
	glBufferSubData ( GL_ARRAY_BUFFER, sizeof ( positions ), 4 * sizeof ( glm::vec3 ), &colours [ 0 ] );
	glVertexAttribPointer ( 3, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) ( sizeof ( positions ) ) );

	//	Texture Coordinates
	unsigned int offset = sizeof ( positions ) + ( 4 * sizeof ( glm::vec3 ) );
	glBufferSubData ( GL_ARRAY_BUFFER, offset, sizeof ( tex_coords ), &tex_coords [ 0 ] );
	glVertexAttribPointer ( 4, 2, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) ( offset ) );

	/*
	 *	Particle Data
	 * */

	//	Setup particle buffer to store each particles position and scale
	glCreateBuffers ( 1, &m_particle_buffer );
	glBindBuffer ( GL_ARRAY_BUFFER, m_particle_buffer );

	//	Allocate memory for all particles data
	glBufferData ( GL_ARRAY_BUFFER, m_num_particles * ( sizeof ( glm::vec3 ) + sizeof ( float ) ), NULL, GL_DYNAMIC_DRAW );

	//	Particle position attribute -- Changes per instance
	glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribDivisor ( 1, 1 );

	//	Particle scale attribute -- Changes per instance
	glVertexAttribPointer ( 2, 1, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) ( m_num_particles * sizeof ( glm::vec3 ) ) );
	glVertexAttribDivisor ( 2, 1 );

	//	Setup alpha buffer
	glCreateBuffers ( 1, &m_alpha_buffer );
	glBindBuffer ( GL_ARRAY_BUFFER, m_alpha_buffer );

	//	Allocate buffer memory
	glBufferData ( GL_ARRAY_BUFFER, m_num_particles * sizeof ( float ), NULL, GL_DYNAMIC_DRAW );

	//	Alpha attribute -- Changes per instance
	glVertexAttribPointer ( 5, 1, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribDivisor ( 5, 1 );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray ( 0 );
}

void ParticleSystem::initialise ( )
{
	this->setup_buffers ( );	
}

void ParticleSystem::update ( const float& p_delta_time )
{
	//	Setup pseudo-random number generator
	static std::mt19937 mt ( 56 );
	std::uniform_real_distribution < float > dist ( m_range.x, m_range.y );

	//	Spawn a number of particles
	for ( int i = 0; i < m_particles_per_frame; ++i )
	{
		//	All particles spawned?
		bool fully_spawned = m_particles.size ( ) == m_num_particles;

		//	Spawn particles if false, break out loop when full
		if ( !fully_spawned )
		{
			Particle new_particle ( m_position, m_scale, m_speed );
			new_particle.m_velocity = m_speed * glm::vec3 ( dist ( mt ), 1.0f, dist ( mt ) );
			new_particle.m_lifetime = this->m_lifetime;
			m_particles.push_back ( new_particle );
		}
		else
		{
			break;
		}
	}

	//	Update each particle
	for ( Particle& particle : m_particles )
	{
		//	Reset if dead and loops 
		if ( particle.m_is_dead && !m_single_burst )
		{
			particle.m_is_dead  = false;
			particle.m_position = this->m_position;
			particle.m_velocity = m_speed * glm::vec3 ( dist ( mt ), 1.0f, dist ( mt ) );
			particle.m_lifetime = this->m_lifetime;
		}

		particle.compute_net_force ( m_use_gravity, m_use_drag, m_use_wind );

		//	Apply net force to particle
		particle.update ( p_delta_time );
	}
}

void ParticleSystem::render ( const Camera& p_camera )
{
	//	Check if texture is being used
	( m_texture_ptr == nullptr ) ? m_colour_shader->begin ( ) : m_texture_shader->begin ( );

	//	Compute view and perspective matrices
	glm::mat4 view  	= glm::lookAt ( p_camera.get_position ( ), glm::vec3 ( 0.0, 0.0, 0.0 ), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );
	glm::mat4 perspective 	= glm::perspective ( glm::radians ( 60.0f ), ( float ) ( 1280 ) / ( float ) ( 720 ), 0.1f, 100.0f );

	//	Get active program
	GLuint shader_program = ( m_texture_ptr == nullptr ) ? m_colour_shader->get_shader_program ( ) : m_texture_shader->get_shader_program ( );

	//	Compute direction to camera
	glm::vec3 direction = p_camera.get_position ( ) - m_position;

	//	Send direction to shader
	GLuint direction_location = glGetUniformLocation ( shader_program, "direction" );
	glUniform3fv ( direction_location, 1, &direction [ 0 ] );

	//	Send matrices to shader
	GLuint view_location = glGetUniformLocation ( shader_program, "view" );
	glUniformMatrix4fv ( view_location, 1, GL_FALSE, &view [ 0 ][ 0 ] );

	GLuint perspective_location = glGetUniformLocation ( shader_program, "perspective" );
	glUniformMatrix4fv ( perspective_location, 1, GL_FALSE, &perspective [ 0 ][ 0 ] );

	//	Buffer each particles data
	std::vector < glm::vec3 >	particle_positions;
	std::vector < float >		scales;
	std::vector < float >		alphas;

	for ( Particle& particle : m_particles )
	{
		particle_positions.push_back ( particle.m_position );
		scales.push_back ( particle.m_scale );

		float alpha = particle.m_lifetime / ( float )( this->m_lifetime );
		alphas.push_back ( alpha );
	}

	/*
	 *	Render
	 * */

	glBindVertexArray ( m_vertex_array );	

	/*
	 *	Buffer Orphaning
	 * */
	
	glBindBuffer ( GL_ARRAY_BUFFER, m_particle_buffer );

	glBufferData ( GL_ARRAY_BUFFER, m_num_particles * ( sizeof ( glm::vec3 ) + sizeof ( float ) ), NULL, GL_DYNAMIC_DRAW );

	//	Store particle positions in first portion of buffer
	glBufferSubData ( GL_ARRAY_BUFFER, 0, particle_positions.size ( ) * sizeof ( glm::vec3 ), particle_positions.data ( ) );

	//	Store particle scales in second portion of buffer
	glBufferSubData ( GL_ARRAY_BUFFER, m_num_particles * sizeof ( glm::vec3 ), scales.size ( ) * sizeof ( float ), scales.data ( ) );

	//	Store alpha values for each particle
	glBindBuffer ( GL_ARRAY_BUFFER, m_alpha_buffer );
	glBufferData ( GL_ARRAY_BUFFER, m_num_particles * sizeof ( float ), NULL, GL_DYNAMIC_DRAW );
	glBufferSubData ( GL_ARRAY_BUFFER, 0, alphas.size ( ) * sizeof ( float ), alphas.data ( ) );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	//	Activate texture if set
	if ( m_texture_ptr )
	{
		glActiveTexture ( GL_TEXTURE0 );
		glBindTexture ( GL_TEXTURE_2D, m_texture_ptr->m_texture_id );
	}

	//	Enable attributes
	glEnableVertexAttribArray ( 0 );
	glEnableVertexAttribArray ( 1 );
	glEnableVertexAttribArray ( 2 );
	glEnableVertexAttribArray ( 3 );
	glEnableVertexAttribArray ( 4 );
	glEnableVertexAttribArray ( 5 );

	//	Perform instanced draw of the current number of particles
	glDrawArraysInstanced ( GL_TRIANGLE_STRIP, 0, 4, m_particles.size ( ) );

	//	Disable attributes
	glDisableVertexAttribArray ( 5 );
	glDisableVertexAttribArray ( 4 );
	glDisableVertexAttribArray ( 3 );
	glDisableVertexAttribArray ( 2 );
	glDisableVertexAttribArray ( 1 );
	glDisableVertexAttribArray ( 0 );

	//	Unbind
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glBindVertexArray ( 0 );

	glUseProgram ( 0 );
}

void ParticleSystem::reset ( )
{
	m_particles.clear ( );
}

void ParticleSystem::set_shaders ( Shader* p_colour_shader, Shader* p_texture_shader )
{
	m_colour_shader  = p_colour_shader;
	m_texture_shader = p_texture_shader;
}

void ParticleSystem::set_position ( const glm::vec3& p_position )
{
	m_position = p_position;
}

void ParticleSystem::set_colour ( const glm::vec3& p_colour )
{
	m_colour = p_colour;
}

void ParticleSystem::set_range ( const glm::vec2& p_range )
{
	m_range = p_range;
}

void ParticleSystem::set_speed ( float p_speed )
{
	m_speed = p_speed;
}

void ParticleSystem::set_scale ( float p_scale )
{
	m_scale = p_scale;
}

void ParticleSystem::set_lifetime ( unsigned int p_lifetime )
{
	m_lifetime = p_lifetime;
}

void ParticleSystem::set_num_particles ( unsigned int p_num_particles )
{
	m_num_particles = p_num_particles;
}

void ParticleSystem::set_particles_per_frame ( unsigned int p_particles_per_frame )
{
	m_particles_per_frame = p_particles_per_frame;
}

void ParticleSystem::set_gravity ( bool p_gravity )
{
	m_use_gravity = p_gravity;
}

void ParticleSystem::set_drag ( bool p_drag )
{
	m_use_drag = p_drag;
}

void ParticleSystem::set_wind ( bool p_wind )
{
	m_use_wind = p_wind;
}

void ParticleSystem::set_burst ( bool p_burst )
{
	m_single_burst = p_burst;
}

void ParticleSystem::set_texture ( Texture* p_texture )
{
	m_texture_ptr = p_texture;
}
