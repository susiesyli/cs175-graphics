#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform sampler2D environMap;
uniform sampler2D objectTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float textureBlend;
uniform bool useDiffuse;

out vec4 outputColor;

vec2 textureLocation(vec3 point) {
    // Calculate view direction in world space

    vec3 p = normalize(point);
    vec3 viewDir = normalize(viewPos - point);
    
    // Calculate reflection direction in world space
    vec3 reflectDir = reflect(-viewDir, normalize(fragNormal));
    
    // Convert reflection vector to spherical coordinates
    // Use atan2 for more robust angle calculation
    float phi = atan(-reflectDir.x, reflectDir.z);
    float theta = acos(reflectDir.y);
    float u = (phi + 3.14159)/ (2.0 * 3.14159) -0.25f;
    float v = theta / (3.14159);


    // Map to texture coordinates
    // Adjust to ensure consistent mapping
    
    // float theta = atan(-reflectDir.z, reflectDir.x) / 2;
    // float phi = -asin(reflectDir.y);
    // float x = (theta + 3.14159) / (3.14159 * 2);
    // float y = (phi / 3.14159) + 0.5f;
    
    vec2 coord = vec2(
        u,
        v
    );
    


    return coord;
}

void main()
{
    // Environment map color
    vec2 envCoord = textureLocation(fragPosition);
    vec4 environColor = texture(environMap, envCoord);
    
    // Object texture color
    vec3 dir = normalize(fragPosition);
    vec2 objTexCoord = vec2(atan(dir.z, dir.x) / (2.0 * 3.14159), -asin(dir.y) / 3.14159 + 0.5);
    vec4 objectColor = texture(objectTexture, objTexCoord);
    
    // Blend textures based on textureBlend
    vec4 finalTexture = mix(environColor, objectColor, textureBlend);
    
    // Diffuse shading
    vec4 diffuseColor = vec4(1.0);
    if (useDiffuse) {
        vec3 norm = normalize(fragNormal);
        vec3 lightDir = normalize(lightPos - fragPosition);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseColor = vec4(diff * vec3(1.0), 1.0);
    }
    
    // Final color combining texture and diffuse shading
    outputColor = finalTexture * diffuseColor;
}