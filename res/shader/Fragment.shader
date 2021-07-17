#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D cubeTexture;
uniform sampler2D wallTexture;
//1 for cube, 0 for wall
uniform int isCubeOrWall;

out vec4 fragColor;

void main(){
    fragColor = isCubeOrWall == 1 ? texture(cubeTexture, TexCoord) : fragColor = texture(wallTexture, TexCoord);
}