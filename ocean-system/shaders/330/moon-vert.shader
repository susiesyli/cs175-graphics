#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 moonModel;
uniform mat4 moonView;
uniform mat4 moonProjection;

out vec3 moonFragPosition;
out vec3 moonFragNormal;

void main()
{
    // Transform vertex position to world space
    moonFragPosition = vec3(moonModel * vec4(myPosition, 1));
    
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    moonFragNormal = mat3(transpose(inverse(moonModel))) * myNormal;
    
    // Compute final vertex position
    gl_Position = moonProjection * moonView * moonModel * vec4(myPosition, 1);
}