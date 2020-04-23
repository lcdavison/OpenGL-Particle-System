#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtx/transform.hpp>

class Camera 
{
	public:

		Camera ( );
		Camera ( const glm::vec3& );

		void 		set_position ( const glm::vec3& );

		void 		set_azimuth  ( float );
		void		set_polar    ( float );
		void		set_distance ( float );

		glm::mat4 	get_view_matrix ( ) 	const;
		glm::vec3 	get_position 	( ) 	const;
		
		float 		get_azimuth 	( ) 	const;
		float 		get_polar   	( )	const;
		float 		get_distance 	( )	const;

	private:

		glm::vec3 	m_position;
		float 		m_azimuth  = 2.0f / M_PI;
		float 		m_polar    = 2.0f / M_PI;
		float 		m_distance = 30.0f;
};
#endif
