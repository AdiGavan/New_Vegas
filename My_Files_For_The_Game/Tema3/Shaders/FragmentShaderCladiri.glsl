#version 330
// "Copyright [2019] Gavan Adrian-George, 334CA"

uniform sampler2D texture1;

in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// Uniforms for spots
uniform vec3 light_position1;
uniform vec3 light_direction1;

uniform vec3 light_position2;
uniform vec3 light_direction2;

uniform vec3 light_position3;
uniform vec3 light_direction3;

uniform vec3 light_position4;
uniform vec3 light_direction4;

uniform int number_spots;

layout(location = 0) out vec4 out_color;

void main()
{
	vec4 color = texture2D(texture1, texcoord);

	if(color.a < 0.5) {
		discard;
	}

	vec3 color_aux;
	
	color = vec4(0, 0, 1, 1);

	// Lumina directionala

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

	float light_total = 0;
	
	float light;
	light = ambient_light + diffuse_light + specular_light;
	light_total = light;
	color_aux = vec3(light_total * color);
	
	// Se incepe calculul pentru lumina spoturilor

	ambient_light = 0;

	// Daca am cel putin un spot
	if (number_spots >= 1) {
		L = normalize( light_position1 - world_position );
		H = normalize( L + V );

		diffuse_light = material_kd * max(dot(N,L), 0);
		specular_light = 0;

		if (diffuse_light > 0)
		{
			specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
		}
		float d = distance(light_position1, eye_position);
		float attenunace_factor = 1 + 0.2 * d + 0.3 * d * d;

		float spot_light = dot(-L, light_direction1);
		float spot_light_limit = cos(30);

		float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
		float light_att_factor = pow(linear_att, 2);

		if (spot_light > cos(30))
		{
			light = ambient_light + light_att_factor * attenunace_factor * ( diffuse_light + specular_light );
		} else  {
			light = ambient_light;
		}
		light_total += light;
		color_aux = vec3(light_total * color);

		// Daca sunt cel putin 2 spoturi
		if (number_spots >= 2) {
			L = normalize( light_position2 - world_position );
			H = normalize( L + V );

			diffuse_light = material_kd * max(dot(N,L), 0);
			specular_light = 0;

			if (diffuse_light > 0)
			{
				specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
			}
			d = distance(light_position2, eye_position);
			attenunace_factor = 1 + 0.2 * d + 0.3 * d * d;

			spot_light = dot(-L, light_direction2);
			spot_light_limit = cos(30);

			linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			light_att_factor = pow(linear_att, 2);

			if (spot_light > cos(30))
			{
				light = ambient_light + light_att_factor * attenunace_factor * ( diffuse_light + specular_light );
			} else  {
				light = ambient_light;
			}
			light_total += light;
			color_aux = vec3(light_total * color);

			// Daca sunt cel putin 3 spoturi
			if (number_spots >= 3) {
				L = normalize( light_position3 - world_position );
				H = normalize( L + V );

				diffuse_light = material_kd * max(dot(N,L), 0);
				specular_light = 0;

				if (diffuse_light > 0)
				{
					specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
				}
				d = distance(light_position3, eye_position);
				attenunace_factor = 1 + 0.2 * d + 0.3 * d * d;

				spot_light = dot(-L, light_direction3);
				spot_light_limit = cos(30);

				linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
				light_att_factor = pow(linear_att, 2);

				if (spot_light > cos(30))
				{
					light = ambient_light + light_att_factor * attenunace_factor * ( diffuse_light + specular_light );
				} else  {
					light = ambient_light;
				}
				light_total += light;
				color_aux = vec3(light_total * color);

				// Daca sunt 4 spoturi
				if (number_spots >= 4) {
					L = normalize( light_position4 - world_position );
					H = normalize( L + V );

					diffuse_light = material_kd * max(dot(N,L), 0);
					specular_light = 0;

					if (diffuse_light > 0)
					{
						specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
					}
					d = distance(light_position4, eye_position);
					attenunace_factor = 1 + 0.2 * d + 0.3 * d * d;

					spot_light = dot(-L, light_direction4);
					spot_light_limit = cos(30);

					linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
					light_att_factor = pow(linear_att, 2);

					if (spot_light > cos(30))
					{
						light = ambient_light + light_att_factor * attenunace_factor * ( diffuse_light + specular_light );
					} else  {
						light = ambient_light;
					}
					light_total += light;
					color_aux = vec3(light_total * color);
				}
			}
		}
	}
	
	out_color = vec4(color_aux, 1);

}