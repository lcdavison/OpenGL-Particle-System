#version 450 core

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 particle_position;
layout ( location = 2 ) in float particle_scale;
layout ( location = 3 ) in vec3 colour;
layout ( location = 5 ) in float alpha;

uniform vec3 direction;
uniform mat4 view;
uniform mat4 perspective;

out float particle_alpha;
out vec3 particle_colour;

void main ( )
{
	particle_alpha = alpha;
	particle_colour = colour;

	vec4 forward = vec4 ( normalize ( direction ), 0.0 );
	vec4 right = vec4 ( cross ( vec3 ( 0.0, 1.0, 0.0 ), forward.xyz ), 0.0 );
	vec4 up = vec4 ( cross ( forward.xyz, right.xyz ), 0.0 );

	mat4 billboard = mat4 ( right, up, forward, vec4 ( 0.0, 0.0, 0.0, 1.0 ) );

	//	Rotate and scale offset vector
	vec4 offset = billboard * vec4 ( ( particle_scale * position ), 0.0 );
	vec4 world_pos = vec4 ( particle_position.xyz + offset.xyz, 1.0 );

	gl_Position = perspective * view * world_pos;
}
