#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace[8];

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texture_sampler;
uniform mat4 MV;

// Allow up to 8 point-source lights.
uniform vec3 LightPosition_worldspace[8];
uniform vec3 LightColour[8];
uniform float LightPower[8];
uniform int PointSourceCount;

uniform vec3 AmbientLightColour;

void main()
{
	// Material properties
	vec3 MaterialDiffuseColour = texture2D(texture_sampler, UV).rgb;
	vec3 MaterialAmbientColour = AmbientLightColour * MaterialDiffuseColour;
	vec3 MaterialSpecularColour = vec3(0.3, 0.3, 0.3);

	// Ambient : simulates indirect lighting
	color = MaterialAmbientColour;

	// Normal of the computed fragment, in camera space
	vec3 n = normalize(Normal_cameraspace);

	for (int i = 0; i < PointSourceCount; ++i)
	{
		// Distance to the light
		float distance = length(LightPosition_worldspace[i] - Position_worldspace);

		// Direction of the light (from the fragment to the light)
		vec3 l = normalize(LightDirection_cameraspace[i]);

		// Cosine of the angle between the normal and the light direction,
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp(dot(n,l), 0,1);

		// Eye vector (towards the camera)
		vec3 E = normalize(EyeDirection_cameraspace);

		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l,n);

		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp(dot(E,R), 0, 1);

		color +=
			// Diffuse : "color" of the object
			MaterialDiffuseColour * LightColour[i] * LightPower[i] * cosTheta / (distance*distance) +
			// Specular : reflective highlight, like a mirror
			MaterialSpecularColour * LightColour[i] * LightPower[i] * pow(cosAlpha,5) / (distance*distance);
	}
}
