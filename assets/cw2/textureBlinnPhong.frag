#version 430

#define PI 3.14159265359

// input
layout(location = 0) in vec3 v2fPosition;
layout(location = 1) in vec3 v2fNormal;
layout(location = 2) in vec2 v2fTexCoord;

//uniform
layout(location = 3) uniform vec3 uGlobalLightDirection;
layout(location = 4) uniform vec3 uGlobalLightAmbient;
layout(location = 5) uniform vec3 uGlobalLightDiffuse;
layout(location = 6) uniform vec3 uViewPosition;

layout(location = 7) uniform vec3 uLightPosition[3];
layout(location = 10) uniform vec3 uLightAmbient[3];
layout(location = 13) uniform vec3 uLightDiffuse[3];

layout(binding = 0) uniform sampler2D uTexture;

// output
layout(location = 0) out vec3 oColor;

// Using Corrected Blinn Phong Model
void main() {
	vec3 texColor = texture(uTexture, v2fTexCoord).rgb;
	
	// Not all data provided by texture - add as needed
	vec3 v2fAmbient = texColor;
	vec3 v2fDiffuse = texColor;
	vec3 v2fSpecular = texColor;
	float v2fShininess = 1;
	vec3 v2fEmissive = vec3(0.0);

	vec3 normal = normalize(v2fNormal);
	vec3 viewDir = normalize(uViewPosition - v2fPosition);

	vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

	for (int i = 0; i < 3; i++) {
		vec3 lightDir = normalize(uLightPosition[i] - v2fPosition);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float dist = length(uLightPosition[i] - v2fPosition);
		float distFactor = 1 / (dist * dist);

		ambient += v2fAmbient * uLightAmbient[i];
		diffuse += v2fDiffuse * uLightDiffuse[i] * max(0.0, dot(normal, lightDir)) * distFactor / PI;
		specular += v2fSpecular * uLightDiffuse[i] * pow(max(0.0, dot(normal, halfwayDir)), v2fShininess) * distFactor * (v2fShininess + 2) / 8;
	}

	// Global Lighting
	ambient += v2fAmbient * uGlobalLightAmbient;
	diffuse += v2fDiffuse * max(0.0, dot(normal, uGlobalLightDirection)) * uGlobalLightDiffuse;

	// Sum of all
	oColor = ambient + diffuse + specular + v2fEmissive;
}