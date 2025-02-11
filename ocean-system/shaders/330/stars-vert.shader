#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 starModel;
uniform mat4 starView;
uniform mat4 starProjection;

out vec3 starFragPosition;
out vec3 starFragNormal;

void main()
{
    // Transform vertex position to world space
    starFragPosition = vec3(starModel * vec4(myPosition, 1));
    
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    starFragNormal = mat3(transpose(inverse(starModel))) * myNormal;
    
    // Compute final vertex position
    gl_Position = starProjection * starView * starModel * vec4(myPosition, 1);
}