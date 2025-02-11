#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
    // Transform vertex position to world space
    fragPosition = vec3(model * vec4(myPosition, 1.0));
    
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    fragNormal = mat3(transpose(inverse(model))) * myNormal;
    
    // Compute final vertex position
    gl_Position = projection * view * model * vec4(myPosition, 1.0);
}