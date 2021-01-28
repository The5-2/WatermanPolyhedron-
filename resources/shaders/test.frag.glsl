#version 330
  
layout(location = 0)  out vec4 out0; // color 

in vec3 oPosition;
in vec3 oColor;
in vec2 oTc;

uniform sampler2D texture;
uniform bool uUseTexture;
uniform bool uUseColor;
uniform vec3 uColor;

void main() { 
	vec3 col;

	if(uUseTexture){
		col = texture2D(texture, oTc).rgb;
	}else if(uUseColor){
		col = uColor;
	}else{
		// col = normalize(oPosition);
		col = oColor;
	}
	
	
	out0 = vec4(col, 1);
}