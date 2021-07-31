#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

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
uniform int toggleLightBox = 0;
uniform int toggleGlow = 0;
uniform sampler2D emissionMap;
uniform sampler2D shadowMap;
uniform float timeValue;

// for debug
uniform float nearPlane;
uniform float farPlane;

// grabbed from stackOverflow
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 getPointLightEffect(PointLight light, vec3 normal, vec3 dirToCamara, float shadow) {
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

    return toggleLightBox == 1 ?
        texture(material.diffuse, TexCoord).rgb :
        ambient * material.ambient + (1.0 - shadow) * (diffuseColor + specularColor);
}

float calculateShadow(vec4 fragPosLightSpace, float bias) {
    // perform perspetive divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current frag from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // no shadow out of frustum in light view
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}

// for debug
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}


void main(){
    vec3 lightEffect;
    vec3 normal = normalize(Normal);

    // shadow
    vec3 lightDir = normalize(pointLight.pos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = calculateShadow(FragPosLightSpace, bias);

    // light
    vec3 dirToCamara = normalize(cameraPos - FragPos);
    lightEffect = getPointLightEffect(pointLight, normal, dirToCamara, shadow);
    vec3 hsvEmissionMap = rgb2hsv(texture(emissionMap, TexCoord).rgb);
    if (toggleGlow == 1) {
        lightEffect += hsv2rgb(vec3(timeValue, hsvEmissionMap.y, hsvEmissionMap.z));
    }

    FragColor = vec4(lightEffect, 1.0);
//    float depthValue = texture(shadowMap, TexCoord).r;
//    FragColor = vec4(vec3(linearizeDepth(depthValue) / farPlane), 1.0);
}