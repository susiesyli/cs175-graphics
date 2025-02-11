#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 cloudModel;
uniform mat4 cloudView;
uniform mat4 cloudProjection;

out vec3 cloudFragPosition;
out vec3 cloudFragNormal;

void main()
{
    // Transform vertex position to world space
    cloudFragPosition = vec3(cloudModel * vec4(myPosition, 1));
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    cloudFragNormal = mat3(transpose(inverse(cloudModel))) * myNormal;
    // Compute final vertex position
    gl_Position = cloudProjection * cloudView * cloudModel * vec4(myPosition, 1);
}