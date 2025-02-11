#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosition;

void main()
{
    // Transform vertex position to world space
    fragPosition = vec3(model * vec4(myPosition, 1.0));
    
    // Compute final vertex position
    gl_Position = projection * view * model * vec4(myPosition, 1.0);
}