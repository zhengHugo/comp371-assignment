#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D cubeTexture;
out vec4 fragColor;

void main(){
    fragColor = texture(cubeTexture, TexCoord), 1.0;
}