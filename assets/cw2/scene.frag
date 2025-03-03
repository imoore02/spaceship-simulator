#version 430

// input
layout(location = 0) in vec3 v2fColor;
layout(location = 1) in vec3 v2fNormal;
layout(location = 2) in vec2 v2fTexCoord;

// uniforms
layout(location = 2) uniform vec3 uLightDir; // should be normalized!
layout(location = 3) uniform vec3 uLightDiffuse;
layout(location = 4) uniform vec3 uSceneAmbient;

layout(binding = 0) uniform sampler2D uTexture;

// output
layout(location = 0) out vec3 oColor;

void main() {
	vec3 normal = normalize(v2fNormal);
	vec3 texColor = texture(uTexture, v2fTexCoord).rgb;
	
	float nDotL = max(0.0, dot(normal, uLightDir));
	oColor = (uSceneAmbient + nDotL * uLightDiffuse) * texColor;
}

