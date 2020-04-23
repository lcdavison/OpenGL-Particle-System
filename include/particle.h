#ifndef PARTICLE_H
#define PARTICLE_H

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_access.hpp>
#include <gtx/string_cast.hpp>

#include <random>

#include "shader.h"
#include "camera.h"

struct Particle 
{
	public:
		Particle ( );
		Particle ( const glm::vec3&, float, float );

		void		update ( const float& );

		void 		compute_net_force ( bool, bool, bool );

		glm::vec3 	m_position;

		glm::vec3	m_velocity;
		glm::vec3	m_acceleration;
		glm::vec3	m_net_force;

		float 		m_mass;
		float 		m_speed;
		float 		m_scale;

		int 		m_lifetime;
		bool		m_is_dead;
};
#endif
