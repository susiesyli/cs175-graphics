#version 330


uniform mat4 myProjectionMatrix;
uniform mat4 myModelviewMatrix;

//pass in the needed variables
in vec3 myNormal;
in vec3 myPosition;

//pass out the needed variables
out vec3 position;
out vec3 normal; 


void main() {
	//TODO: modify this code to make the object appear!
    //vec4 tmpPos = myProjectionMatrix * myModelviewMatrix * vec4(myPosition, 0.0);
	//gl_Position = vec4(tmpPos.xyz, 1.0);
    //vec3 normal = normalize(tmpPos.xyz);

    gl_Position = myProjectionMatrix * myModelviewMatrix * vec4(myPosition, 1.0);
    position = vec3(myModelviewMatrix * vec4(myPosition, 1.0));
    normal = mat3(myModelviewMatrix) * myNormal;
}

