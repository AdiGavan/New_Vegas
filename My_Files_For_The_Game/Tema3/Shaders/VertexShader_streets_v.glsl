#version 330
// "Copyright [2019] Gavan Adrian-George, 334CA"

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float time;
uniform int rotation;

out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{	
	world_position =  vec3( Model * vec4(v_position,1) );
	world_normal = normalize( mat3(Model) * v_normal );
	texcoord = v_texture_coord * vec2(1, 50);
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
