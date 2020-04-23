#include "particle.h"

Particle::Particle ( ) : 
	m_position 	( glm::vec3 ( 0.0f, 0.0f, -1.0f ) ), 
	m_mass 		( 1.0f ), 
	m_scale 	( 0.1f ), 
	m_lifetime 	( 255 ), 
	m_is_dead 	( false ) { }

Particle::Particle ( const glm::vec3& p_position, float p_scale, float p_speed ) : 
	m_position 	( p_position ), 
	m_speed 	( p_speed ), 
	m_mass 		( 1.0f ), 
	m_scale 	( p_scale ), 
	m_lifetime 	( 255 ), 
	m_is_dead 	( false ) { }

void Particle::update ( const float& p_delta_time )
{
	//	Is particle dead?
	if ( m_lifetime <= 0 )	m_is_dead = true;

	//	Compute acceleration
	m_acceleration = m_net_force / m_mass;

	//	Update particle position
	m_velocity += m_acceleration * p_delta_time;
	m_position += m_velocity * p_delta_time;

	m_speed = glm::length ( m_velocity );

	//	Decrease lifetime
	m_lifetime--;
}

void Particle::compute_net_force ( bool p_gravity, bool p_drag, bool p_wind )
{
	//	Reset force
	m_net_force.x = 0.0f;
	m_net_force.y = 0.0f;
	m_net_force.z = 0.0f;

	//	Gravity enabled?
	if ( p_gravity )
	{
		//	Add gravity
		m_net_force += m_mass * glm::vec3 ( 0.0f, -9.8f, 0.0f );
	}

	//	Area of the particle - treated as a circle
	float area = M_PI * m_scale * m_scale;

	if ( p_drag )
	{
		//	Add drag
		float drag_coeff = 0.5f * 1.23f * ( m_speed * m_speed ) * area * 0.6f;
		glm::vec3 drag = -m_velocity;
		drag = glm::normalize ( drag );

		drag *= drag_coeff;

		m_net_force += drag;
	}

	//	Wind enabled?
	if ( p_wind )
	{
		//	Simulate wind
		double cur_time = glfwGetTime ( );
		static double last_time = 0.0;

		//	Has 10 seconds passed?
		if ( cur_time - last_time > 10.0 )
		{
			float wind_coeff = 0.5f * 1.23f * ( 5.0f * 5.0f ) * area * 0.6f;
			glm::vec3 wind = glm::vec3 ( -1.0f, 0.0f, 0.0f ) * wind_coeff;

			m_net_force += wind;

			//	Stop after 5 seconds
			if ( cur_time - last_time > 15.0 )
			{
				last_time = cur_time;
			}
		}
	}
}
