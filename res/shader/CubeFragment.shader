#version 330 core
in vec2 TexCoord;

uniform sampler2D cubeTexture;

out vec4 fragColor;

void main(){
    fragColor = texture(cubeTexture, TexCoord), 1.0;
}