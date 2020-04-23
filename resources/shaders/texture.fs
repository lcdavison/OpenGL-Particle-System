#version 450 core

in float particle_alpha;
in vec2 tex_coords;

out vec4 fragColour;

uniform sampler2D particle_texture;

void main ( )
{
	vec4 texel_colour = texture ( particle_texture, tex_coords );

	if ( texel_colour.a < 0.5 )
		discard;

	fragColour = vec4 ( 1.0, 1.0, 1.0, particle_alpha ) * texel_colour;
}
