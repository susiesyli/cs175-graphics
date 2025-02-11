#version 330

layout(location = 0) in vec3 myPosition;
layout(location = 1) in vec3 myNormal;

uniform mat4 rainModel;
uniform mat4 rainView;
uniform mat4 rainProjection;

out vec3 rainFragPosition;

void main()
{
    rainFragPosition = vec3(rainModel * vec4(myPosition, 1.0));
    gl_Position = rainProjection * rainView * vec4(rainFragPosition, 1.0);
}