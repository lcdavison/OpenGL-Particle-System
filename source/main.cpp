#include "window.h"
#include "particle_system.h"
#include "texture_loader.h"

#include <cmath>

static Camera _camera;

static Shader 		_shaders [ 2 ];
static Texture* 	_fire_texture 	  = nullptr;
static Texture*		_firework_texture = nullptr;

static ParticleSystem 	_particle_systems [ 3 ];
static ParticleSystem* 	_active_system = nullptr;

static unsigned int 	_system_index = 0;

void adjust_distance ( GLFWwindow* p_window, double p_xoffset, double p_yoffset )
{
	//	Compute new distance
	float distance = _camera.get_distance ( ) - p_yoffset;

	_camera.set_distance ( glm::clamp ( distance, 10.0f, 70.0f ) );
}

void change_particle_system ( GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods )
{
	//	Has particle system changed?
	bool change = false;

	if ( p_key == GLFW_KEY_N && p_action == GLFW_PRESS )
	{
		//	Maximum index is reached?
		bool maximum = _system_index == 2;

		//	Increment if not, otherwise add nothing
		_system_index += ( maximum ) ? 0 : 1;

		//	Should update current particle system?
		change = !maximum;
	}

	if ( p_key == GLFW_KEY_B && p_action == GLFW_PRESS )
	{
		//	Minimum index is reached?
		bool minimum = _system_index == 0;

		//	Decrement if not, otherwise subtract nothing
		_system_index -= ( minimum ) ? 0 : 1;

		//	Should update current particle system?
		change = !minimum;
	}

	//	Should particle system change?
	if ( change )
	{
		//	Reset the current particle system
		_active_system->reset ( );

		//	Update active particle system
		_active_system = &_particle_systems [ _system_index ];	
	}
}

void create_particle_systems ( )
{
	//	Setup single burst particle system
	ParticleSystem& burst_system = _particle_systems [ 0 ];

	burst_system.set_position 	( glm::vec3 ( 0.0f, 0.0f, 0.0f ) );	//	Position
	burst_system.set_colour 	( glm::vec4 ( 0.0, 0.0, 1.0, 1.0 ) );	//	Colour
	burst_system.set_num_particles 	( 1000 );				//	Num particles
	burst_system.set_speed 		( 40.0f );				//	Particle speed
	burst_system.set_gravity 	( true );				//	Gravity?
	burst_system.set_burst 		( true );				//	Single burst?

	burst_system.initialise ( );

	//	Setup fire particle system
	ParticleSystem& fire = _particle_systems [ 1 ];

	fire.set_position 	( glm::vec3 ( 0.0f, 0.0f, 0.0f ) );		//	Position
	fire.set_scale 		( 1.0f );
	fire.set_lifetime	( 128 );
	fire.set_colour 	( glm::vec4 ( 1.0, 0.0, 0.0, 1.0 ) );		//	Colour
	fire.set_num_particles 	( 1000 );					//	Num particles
	fire.set_speed 		( 8.0f );					//	Particle speed
	fire.set_range 		( glm::vec2 ( -0.3f, 0.3f ) );			//	Emitter range
	fire.set_gravity 	( false );					//	Gravity?
	fire.set_drag		( false );					//	Drag?
	fire.set_wind 		( true );					//	Wind?
	fire.set_burst 		( false );					//	Single burst?
	fire.set_texture 	( _fire_texture );				//	Texture

	fire.initialise ( );

	//	Setup explosions particle system
	ParticleSystem& explosions = _particle_systems [ 2 ];

	explosions.set_position 	   ( glm::vec3 ( 0.0f, 0.0f, 0.0f ) );	//	Position
	explosions.set_colour 		   ( glm::vec4 ( 1.0, 1.0, 0.0, 1.0 ) );//	Colour
	explosions.set_num_particles 	   ( 5000 );				//	Num particles
	explosions.set_speed 		   ( 30.0f );				//	Particle speed
	explosions.set_particles_per_frame ( 100 );				//	Particles per frame
	explosions.set_gravity 		   ( true );				//	Gravity?
	explosions.set_wind 		   ( false );				//	Wind?
	explosions.set_burst 		   ( false );				//	Single burst?
	explosions.set_texture 		   ( _firework_texture );		//	Texture

	explosions.initialise ( );

	//	Set active particle system
	_active_system = &_particle_systems [ _system_index ];
}

bool setup_shaders ( )
{
	//	Accumulates with compile errors
	short result = 0;

	result += 1 - _shaders [ 0 ].compile ( "./shaders/colour.vs", "./shaders/colour.fs" );
	result += 1 - _shaders [ 1 ].compile ( "./shaders/texture.vs", "./shaders/texture.fs" );

	//	When greater than zero, compilation failed
	if ( result > 0 )
		return false;

	//	Set shaders for use by particle systems
	for ( int i = 0; i < 3; ++i )
	{
		_particle_systems [ i ].set_shaders ( &_shaders [ 0 ], &_shaders [ 1 ] );	
	}

	return true;
}

glm::vec3 spherical_to_cartesian ( float p_radius, float p_polar, float p_azimuth )
{
	return glm::vec3 ( p_radius * std::sin ( p_azimuth ) * std::sin ( p_polar ), 
			   p_radius * std::cos ( p_azimuth ), 
			   p_radius * std::sin ( p_azimuth ) * std::cos ( p_polar ) );
}

int main ( )
{
	//	Create window
	Window test ( 1280, 720 );

	//	Disable cursor
	glfwSetInputMode ( test.get_glfw_ptr ( ), GLFW_CURSOR, GLFW_CURSOR_DISABLED );

	//	Setup scroll wheel distance control
	glfwSetScrollCallback 	( test.get_glfw_ptr ( ), adjust_distance );

	//	Setup controls for changing active particle system
	glfwSetKeyCallback 	( test.get_glfw_ptr ( ), change_particle_system );

	//	Load textures
	{
		TextureLoader loader ( "./textures/fire.png" );
		bool result = loader.load ( );

		if ( !result )
		{
			std::cout << "Failed to load texture : fire.png" << std::endl;
			return -1;
		}

		_fire_texture = loader.get_texture ( );
	}

	{
		TextureLoader loader ( "./textures/firework.png" );
		bool result = loader.load ( );

		if ( !result )
		{
			std::cout << "Failed to load texture : firework.png" << std::endl;
			return -1;
		}

		_firework_texture = loader.get_texture ( );
	}
	
	create_particle_systems ( );

	if ( !setup_shaders ( ) )
	{
		std::cerr << "Shaders failed to compile" << std::endl;
		return 1;
	}

	//	Create _camera
	_camera = Camera ( glm::vec3 ( 10.0f, 10.0f, 0.0f ) );

	//	Setup OpenGL
	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );

	//	Enable alpha blending
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//	Clear screen to black
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );

	//	Use VSync
	glfwSwapInterval ( 1 );

	//	Setup physics timer - timer uses milliseconds
	const GLFWvidmode* video_mode = glfwGetVideoMode ( glfwGetPrimaryMonitor ( ) );

	float accumulator = 0.0f;
	float interval = 1000.0f / ( float ) ( video_mode->refreshRate );
	float delta_time = interval / 1000.0f;	//	Measured in seconds

	//	Mouse position for _camera control
	double mouse_x, mouse_y;
	glfwGetCursorPos ( test.get_glfw_ptr ( ), &mouse_x, &mouse_y );

	//	Used for detecting reset key press
	bool previous_state = GLFW_RELEASE;

#ifdef DEBUG
	unsigned int frames = 0;
	double prev_time = glfwGetTime ( );
#endif

	while ( !glfwWindowShouldClose ( test.get_glfw_ptr ( ) ) )
	{
		float start = glfwGetTime ( ) * 1000.0f;

		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//	Perform a physics update after interval milliseconds
		while ( accumulator >= interval )
		{
			accumulator -= interval;

			//	Update active particle system
			_active_system->update ( delta_time );

			//	Position _camera around the particle system
			double x, y;
			glfwGetCursorPos ( test.get_glfw_ptr ( ), &x, &y );

			float polar 	= _camera.get_polar ( ) + glm::radians ( ( x - mouse_x ) * delta_time );
			float azimuth 	= _camera.get_azimuth ( ) + glm::radians ( ( mouse_y - y ) * delta_time );

			//	Wrap the polar angle
			float two_pi = 2.0f * M_PI;
			_camera.set_polar ( polar - std::floor ( polar / two_pi ) * two_pi );
			
			//	Clamp the azimuthal angle
			_camera.set_azimuth ( glm::clamp ( azimuth, glm::radians ( 60.0f ), glm::radians ( 120.0f ) ) );

			mouse_x = x;
			mouse_y = y;

			//	Set _camera position using Spherical Coordinates
			_camera.set_position ( glm::vec3 ( 0.0f, 0.0f, 0.0f ) + spherical_to_cartesian ( _camera.get_distance ( ), _camera.get_polar ( ), _camera.get_azimuth ( ) ) );

			//	Reset particle system when R key is pressed
			bool current_state = glfwGetKey ( test.get_glfw_ptr ( ), GLFW_KEY_R );
			if ( current_state == GLFW_PRESS && previous_state == GLFW_RELEASE )
			{
				_active_system->reset ( );
			}

			previous_state = current_state;
		}

		//	Render particle system
		_active_system->render ( _camera );

		//	Present back buffer
		glfwSwapBuffers ( test.get_glfw_ptr ( ) );

		//	Check for GLFW events
		glfwPollEvents ( );

		//	Accumulate frame duration
		accumulator += ( glfwGetTime ( ) * 1000.0f ) - start;

#ifdef DEBUG
		frames++;

		double cur_time = glfwGetTime ( );
		if ( ( cur_time - prev_time ) > 1.0 )
		{
			std::printf ( "FPS : %3.d\r", frames );
			frames = 0;
			prev_time = cur_time;
		}
#endif
	}

	return 0;
}
