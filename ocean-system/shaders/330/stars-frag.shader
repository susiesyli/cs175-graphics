#version 330

in vec3 fragPosition;
in vec3 fragNormal;

// uniform sampler2D environMap;
uniform vec3 lightPos; 
uniform float lightIntensity;  // Link to global star light intensity
// uniform vec3 starColor; 

out vec4 outputColor;

void main()
{	
    vec3 starColor = vec3(1.0f, 0.9f, 0.9f); // without alpha first 
    // float skyBrightness = clamp(lightIntensity * (0.1 + 0.5 * sunDir.y), 0.0, 1.0); // darker as sun falls 

    // stars should be more visible as sky gets dark 
    // sky brightness is the smaller value between (1 - light intensity) or (as lower as sun goes)

    // vec3 viewDir = normalize(fragPosition); 
    // vec3 sunDir = normalize(lightPos);   
    // float sunAngle = max(dot(viewDir, sunDir), 0.0);

    // float sunVisibility = clamp(sunDir.y, 0.0, 1.0);  //step(0.0, sunDir.y);
    // float sunGlow = pow(sunAngle, 500.0) * lightIntensity; // falloff value 
    // vec3 sunContribution = vec3(sunGlow);   // ! change sun color later? 

    // tune sky brightness based on light intensity and sun position
    // float skyBrightness = clamp(lightIntensity * (0.1 + 0.5 * sunDir.y), 0.0, 1.0); // darker as sun falls 


    // float skyBrightness = min(clamp(lightIntensity * (0.1 + 0.5 * sunDir.y), 0.0, 1.0), (1-lightIntensity));
    // float sunDarkness = 1.0 - sunVisibility; 
    float lightDarkness = 1.0 - lightIntensity;

    // float skyDarkness = min(sunDarkness, lightDarkness);

    // float starVisibility = pow(skyDarkness, 1.5);

    outputColor = vec4(starColor, lightDarkness);
    // Sample the environment map using spherical coordinates
    // vec2 texCoord = textureLocation(fragPosition);
    // outputColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
}