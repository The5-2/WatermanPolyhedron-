#version 330

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform float uGlPointSize;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

out vec3 oPosition;
out vec3 oColor;

void main(){
	oPosition = vPosition;
	oColor = vColor;
	
	gl_PointSize = uGlPointSize;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(vPosition, 1.0);
}