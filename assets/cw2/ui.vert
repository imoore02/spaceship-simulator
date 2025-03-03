#version 430

// Input Data
layout(location = 0) in vec3 iPosition;


void main() {
    gl_Position = vec4(iPosition, 1.0);
}