#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform float lightIntensity;

out vec4 outputColor;

void main()
{	
    vec3 sunColor = vec3(1.0f, 0.9f, 0.7f); 
    vec3 scaledSunColor = sunColor * lightIntensity;
    outputColor = vec4(scaledSunColor, lightIntensity);
}