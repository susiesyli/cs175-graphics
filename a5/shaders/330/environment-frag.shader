#version 330

in vec3 fragPosition;

uniform sampler2D environMap;

out vec4 outputColor;

vec2 textureLocation(vec3 point) {
    // Simple spherical mapping for environment sphere
    vec3 norm = normalize(point);
    
    // Convert to spherical coordinates
    float phi = -atan(norm.z, -norm.x);
    float theta = acos(norm.y);
    
    // Map to texture coordinates
    vec2 coord = vec2(
        0.5 + phi / (2.0 * 3.14159),
        theta / 3.14159
    );
    
    return coord;
}

void main()
{	
    // Sample the environment map using spherical coordinates
    vec2 texCoord = textureLocation(fragPosition);
    outputColor = texture(environMap, texCoord);
}