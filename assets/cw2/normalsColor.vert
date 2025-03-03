#version 430

// Input Data
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;

// uniform
layout(location = 0) uniform mat4 uProjCameraWorld;
layout(location = 1) uniform mat3 uNormalMatrix;

// Outputs
layout(location = 0) out vec3 v2fColor;

void main() {
    v2fColor = normalize(uNormalMatrix * iNormal);
    gl_Position = uProjCameraWorld * vec4(iPosition, 1.0);
}