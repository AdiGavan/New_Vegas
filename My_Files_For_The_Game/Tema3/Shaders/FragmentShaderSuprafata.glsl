#version 330
// "Copyright [2019] Gavan Adrian-George, 334CA"

uniform sampler2D texture1;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

layout(location = 0) out vec4 out_color;

void main()
{
	vec4 color = texture2D(texture1, texcoord);

	if(color.a < 0.5) {
		discard;
	}

	vec3 color_aux;

	vec3 N = normalize( world_normal );
	vec3 L = normalize( light_position - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );

	float ambient_light = material_kd * 0.25;

	float diffuse_light = material_kd * max(dot(N,L), 0);

	float specular_light = 0;

	if (diffuse_light > 0)
	{
		specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
	}

	float light;
	light = ambient_light + diffuse_light + specular_light;
	color_aux = vec3(light * color);	
	out_color = vec4(color_aux, 1);
}