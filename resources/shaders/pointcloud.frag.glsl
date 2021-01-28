#version 330
  
layout(location = 0) out vec4 out0; // color 

in vec3 oPosition;
in vec3 oColor;

void main() { 
	out0 = vec4(oColor, 1);
}