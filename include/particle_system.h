#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>

#include "particle.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"

class ParticleSystem 
{
	public:

		ParticleSystem ( );
		ParticleSystem ( const glm::vec3&, const glm::vec3&, unsigned int );

		void 				update ( const float& );
		void 				render ( const Camera& );

		void				initialise ( );
		void 				reset ( );

		void				set_shaders  ( Shader*, Shader* );

		void				set_position ( const glm::vec3& );
		void				set_colour   ( const glm::vec3& );
		void				set_range    ( const glm::vec2& );

		void				set_speed 		( float );
		void				set_scale    		( float );
		void				set_lifetime		( unsigned int );
		void				set_num_particles 	( unsigned int );
		void				set_particles_per_frame ( unsigned int );

		void				set_gravity  ( bool );
		void				set_drag     ( bool );
		void 				set_wind     ( bool );
		void				set_burst    ( bool );
		void				set_texture  ( Texture* );

	private:

		void 				setup_buffers ( );

		std::vector < Particle > 	m_particles;

		glm::vec3			m_colour;			//	Particle colour
		glm::vec3 			m_position;			//	Position of emitter

		glm::vec2 			m_range;			//	Controls spread of velocities

		Shader* 			m_texture_shader;		//	Shader for texturing
		Shader*				m_colour_shader;		//	Shader for colouring

		Texture*			m_texture_ptr = nullptr;	//	Texture for particles

		GLuint				m_vertex_array;			//	Stores data related to buffers
		GLuint				m_vertex_buffer;		//	Stores quad vertices
		GLuint				m_alpha_buffer;			//	Stores particle alpha values over lifetime
		GLuint 				m_particle_buffer;		//	Stores particle positions and scales

		float				m_speed 	= 10.0f;	//	Speed of particles
		float 				m_scale 	= 0.5f;		//	Scale of particles
		unsigned int			m_lifetime 	= 255;		//	Lifespan of particles
		unsigned int			m_num_particles;
		unsigned int 			m_particles_per_frame = 1;	//	Controls the number of particles emitted per frame

		bool				m_use_gravity 	= true;		//	Does a particle simulate gravity?
		bool				m_use_drag	= true;		//	Does particle experience drag?
		bool				m_use_wind 	= false;	//	Is particle effected by wind?
		bool				m_single_burst 	= false;	//	Does the particle system only emit once?
};
#endif
