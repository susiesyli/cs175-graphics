#version 330

in vec3 fragPosition; // The fragment's position in world space
in vec3 fragNormal;   // The fragment's normal vector in world space

uniform sampler2D moonMap; // The moon's texture map

out vec4 outputColor;

void main()
{
    outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
