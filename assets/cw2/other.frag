#version 430

// input
layout(location = 0) in vec3 v2fColor;
layout(location = 1) in vec3 v2fNormal;

// output
layout(location = 0) out vec3 oColor;

void main() {
	vec3 normal = normalize(v2fNormal);
	oColor = normal;
}

