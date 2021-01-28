#version 330
  
layout(location = 0)  out vec4 out0; // color 

in vec2 tc;
uniform sampler2D texture;

void main() 
{ 
	vec3 col = texture2D(texture, tc).rgb;
	out0 = vec4(col, 1);
}