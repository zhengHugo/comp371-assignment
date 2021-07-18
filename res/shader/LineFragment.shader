#version 330 core

in vec3 vertexColor;
out vec4 FragColor;

uniform vec3 myColor = vec3(1.0f);

void main(){
    FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
    if (myColor != vec3(1.0f)){
        FragColor.rgb =  myColor;
    }
}