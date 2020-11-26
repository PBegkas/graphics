#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 texCoord;

out vec4 fragmentColor;
out vec2 fragmentTexCoord;

uniform mat4 MVP;
void main(){
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    fragmentColor = vertexColor;
    fragmentTexCoord = texCoord;
}