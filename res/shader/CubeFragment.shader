#version 330 core
in vec2 TexCoord;

uniform sampler2D aTexture;

out vec4 fragColor;

void main(){
    fragColor = texture(aTexture, TexCoord), 1.0;
}