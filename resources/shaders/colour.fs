#version 450 core

in float particle_alpha;
in vec3 particle_colour;

out vec4 fragColour;

void main ( )
{
	fragColour = vec4 ( particle_colour, particle_alpha );
}
