#version 330 core

in vec3 rainFragPosition;  
in vec3 rainFragNormal;  

out vec4 outputColor;

void main() {



    vec3 color = vec3(0.8f, 0.9f, 0.9f); 
    color *= 0.8;

    outputColor = vec4(color, 0.5f);
}
