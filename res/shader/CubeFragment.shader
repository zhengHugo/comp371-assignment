#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct PointLight {
    vec3 pos;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform PointLight pointLight;

uniform vec3 ambientColor;
uniform vec3 cameraPos;


vec3 getPointLightEffect(PointLight light, vec3 normal, vec3 dirToCamara) {
    vec3 dirToLight = normalize(light.pos - FragPos);
    // attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear* dist + light.quadratic * (dist * dist));

    //diffuse
    float diffuseIntensity = max(dot(dirToLight, normal), 0) * attenuation;
    vec3 diffuseColor = diffuseIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

    //specular
    vec3 reflectVec = reflect(-dirToLight, normal);
    float specularIntensity = pow(max(dot(reflectVec, dirToCamara), 0.0), material.shininess) * attenuation;
    vec3 specularColor = specularIntensity * light.color * texture(material.specular, TexCoord).rgb;

    //ambient
    vec3 ambient = ambientColor * texture(material.diffuse, TexCoord).rgb;
    vec3 result = ambient*material.ambient + diffuseColor + specularColor;
    return result;
}

void main(){
    vec3 lightEffect;
    vec3 normal = normalize(Normal);
    vec3 dirToCamara = normalize(cameraPos - FragPos);

    lightEffect = getPointLightEffect(pointLight, normal, dirToCamara);
    FragColor = vec4(lightEffect, 1.0);
}