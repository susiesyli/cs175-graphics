#version 330

uniform mat4 myProjectionMatrix;
uniform mat4 myModelviewMatrix;

in vec3 myNormal;
in vec3 myPosition;

out vec4 position;
out vec3 normal; 

void main() {
	//TODO: modify this code to make the object appear!
	
    vec4 tmpPos = myProjectionMatrix * myModelviewMatrix * vec4(myPosition, 0.0);
	gl_Position = vec4(tmpPos.xyz, 1.0);
    vec3 normal = normalize(tmpPos.xyz);

}

