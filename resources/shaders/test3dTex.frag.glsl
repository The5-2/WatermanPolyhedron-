#version 330

//Requires: OES_texture_3D
//https://www.khronos.org/registry/OpenGL/extensions/OES/OES_texture_3D.txt  
layout(location = 0)  out vec4 out0; // color 

in vec2 tc;

uniform sampler3D tex3d;
uniform int texLevel;

void main() { 
	vec3 texPos= vec3(tc, texLevel);

	vec3 col;
	col = texture(tex3d, texPos).rgb;

	out0 = vec4(col, 1);
}