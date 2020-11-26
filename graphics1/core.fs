#version 330 core
in vec4 fragmentColor;
in vec2 fuv;

out vec4 color;

uniform sampler2D myTextureSampler;
void main(){
	color = texture( myTextureSampler, fuv ) * fragmentColor;
}