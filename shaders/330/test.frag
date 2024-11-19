#version 330

//TODO: pass in the needed variables

in vec4 position;
in vec3 normal;

out vec4 outputColor; //this tells OpenGL that this variable is the output color

void main()
{
	//TODO: goal is to set outputColor to the right info


    vec3 light = vec3(1.0, 1.0, 1.0);
    float diffuse = dot(normalize(light), normal);
    

	//outputColor =  vec4(, 1.0, 1.0, 1.0);
    outputColor = vec4(max(diffuse, 0.0) * normal, 1.0);
}