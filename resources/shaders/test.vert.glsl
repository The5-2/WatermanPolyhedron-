#version 330

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

layout(location = 0) in  vec3 vPosition; 
layout(location = 1) in  vec3 vColor; 

out vec3 oPosition;
out vec3 oColor;
out vec2 oTc;

void main(){
	oPosition = vPosition;
	oTc = vPosition.xy * vec2(0.5) + vec2(0.5);
	oColor = vColor;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(vPosition, 1.0);
}