#version 330

in vec3 fragPosition;

uniform sampler2D environMap;
uniform vec3 lightPos;
uniform float lightIntensity;
out vec4 outputColor;

// spherical mapping for environment sphere
vec2 textureLocation(vec3 point) {
    vec3 norm = normalize(point);
    // convert to spherical coordinates
    float phi = -atan(norm.z, -norm.x);
    float theta = acos(norm.y);
    // map to texture coordinates
    vec2 coord = vec2(0.5 + phi / (2.0 * 3.14159), theta / 3.14159);
    return coord;
}

vec3 calculateEnvironmentColor(vec3 lightDirection) {
    float elevation = clamp(normalize(lightDirection).y, -0.2, 1.0);

    vec3 sunriseColor = vec3(1.0, 0.5, 0.3);
    vec3 dayColor = vec3(0.4, 0.7, 1.0);
    vec3 sunsetColor = vec3(0.8, 0.3, 0.2);
    vec3 nightColor = vec3(0.05, 0.05, 0.2);

    vec3 environmentColor;
    if (elevation > 0.2) {
        // Day to sunrise/sunset
        float t = smoothstep(0.2, 0.5, elevation);
        environmentColor = mix(sunriseColor, dayColor, t);
    } else if (elevation > -0.1) {
        // Sunset to night
        float t = smoothstep(-0.1, 0.2, elevation);
        environmentColor = mix(sunsetColor, nightColor, t);
    } else {
        // Nighttime
        environmentColor = nightColor;
    }

    return environmentColor;
}

void main()
{	
    bool tintSky = false;
    // environment map - texture etc 
    vec2 texCoord = textureLocation(fragPosition);
    vec3 baseSkyColor = texture(environMap, texCoord).rgb;

    // find sunlight contributino 
    vec3 viewDir = normalize(fragPosition); 
    vec3 sunDir = normalize(lightPos);   
    float sunAngle = max(dot(viewDir, sunDir), 0.0);

    float sunVisibility = step(0.0, sunDir.y);
    float sunGlow = pow(sunAngle, 500.0) * lightIntensity;
    vec3 sunContribution = vec3(sunGlow);

    // tune sky brightness based on light intensity and sun position
    float skyBrightness = clamp(lightIntensity * (0.1 + 0.5 * sunDir.y), 0.0, 1.0);

    if (tintSky) {
        // Combine base sky color, tint, and sunlight contribution
        vec3 skyTint = calculateEnvironmentColor(lightPos);
        vec3 tintedSkyColor = mix(baseSkyColor, skyTint, 0.15);
        vec3 finalSkyColor = tintedSkyColor * skyBrightness + sunContribution;
        outputColor = vec4(finalSkyColor, 1.0);
    } else {
        vec3 finalSkyColor = baseSkyColor * skyBrightness + sunContribution;
        outputColor = vec4(finalSkyColor, 1.0);
    }
}