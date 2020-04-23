#include "camera.h"

Camera::Camera ( ) { }

Camera::Camera ( const glm::vec3& p_position ) : m_position ( p_position ) { }

glm::mat4 Camera::get_view_matrix ( ) const
{
	return glm::translate ( glm::mat4 ( 1.0f ), m_position );
}

void Camera::set_position ( const glm::vec3& p_position )
{
	m_position = p_position;
}

void Camera::set_azimuth ( float p_azimuth )
{
	m_azimuth = p_azimuth;
}

void Camera::set_polar ( float p_polar )
{
	m_polar = p_polar;
}

void Camera::set_distance ( float p_distance )
{
	m_distance = p_distance;
}

glm::vec3 Camera::get_position ( ) const
{
	return m_position;
}

float Camera::get_azimuth ( ) const
{
	return m_azimuth;
}

float Camera::get_polar ( ) const
{
	return m_polar;
}

float Camera::get_distance ( ) const
{
	return m_distance;
}
