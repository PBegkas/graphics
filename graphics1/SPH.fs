#version 330 core
in vec2 UV;
in vec4 fragmentColor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){

	color = vec3(1,1,0,1);;
	
}