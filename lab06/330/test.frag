#version 330

uniform vec3 lightPosition;
uniform vec3 lightColor;


//TODO: pass in the needed variables
in vec3 position;
in vec3 normal; 

out vec4 outputColor; //this tells OpenGL that this variable is the output color

void main()
{
	//TODO: goal is to set outputColor to the right info
    vec3 thisNormal = normalize(normal);
    vec3 lightDir = normalize(lightPosition - position);
    
    float diffuse = max(dot(thisNormal, lightDir), 0.0);
    float finalLightIntensity = diffuse; 
    
    // use this rgb panel for tuning light color 
    // vec3 lightColor = vec3(1.0, 1.0, 1.0); // standard white light 
    // vec3 lightColor = vec3(0.0, 0.0, 1.0); // dark blue 
    // vec3 lightColor = vec3(0.5, 0.5, 1.0); // violet  
    // vec3 lightColor = vec3(0.5, 0.5, 1.0); // violet  

    // Final color (using white light)
    outputColor = vec4(lightColor * finalLightIntensity, 1.0);
}