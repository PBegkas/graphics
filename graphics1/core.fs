#version 330 core
in vec4 fragmentColor;
out vec4 color;
void main(){
	color = fragmentColor;
	color.a = 0.5;
}