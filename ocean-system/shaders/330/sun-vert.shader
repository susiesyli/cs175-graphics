#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 sunModel;
uniform mat4 sunView;
uniform mat4 sunProjection;

out vec3 sunFragPosition;
out vec3 sunFragNormal;

void main()
{
    // Transform vertex position to world space
    sunFragPosition = vec3(sunModel * vec4(myPosition, 1));
    
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    sunFragNormal = mat3(transpose(inverse(sunModel))) * myNormal;
    
    // Compute final vertex position
    gl_Position = sunProjection * sunView * sunModel * vec4(myPosition, 1);
}