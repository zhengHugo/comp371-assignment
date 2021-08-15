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

struct SpotLight {
    vec3 pos;
    vec3 lightDir;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float innerCosPhy;
    float outerCosPhy;
};

struct DirectionalLight {
    vec3 pos;
    vec3 lightDir;
    vec3 color;
};

uniform Material material;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform DirectionalLight directionalLight;

uniform vec3 ambientColor;
uniform vec3 cameraPos;
uniform bool isLightBox = false;
uniform bool isFlowing = false;
uniform bool isGlowingOn = false;
uniform bool isTextureOn = true;
uniform bool isShadowOn = true;
uniform sampler2D emissionMap;
uniform sampler2D shadowMap;
uniform float timeValuePeriod;
uniform float timeValueIncrement;

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
    vec3 diffuseTexture, specularTexture;
    if (isTextureOn){
        if(isFlowing){
            diffuseTexture = texture(material.diffuse, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
            specularTexture = texture(material.specular, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
        }else{
            diffuseTexture = texture(material.diffuse, TexCoord).rgb;
            specularTexture = texture(material.specular, TexCoord).rgb;
        }
    } else {
        diffuseTexture = vec3(1);
        specularTexture = vec3(1);
    }

    vec3 dirToLight = normalize(light.pos - FragPos);
    //due to unreal light reflex, using Blinn-Phong
    vec3 halfwayDir = normalize(dirToLight+dirToCamara);

    // attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear* dist + light.quadratic * (dist * dist));

    //diffuse
    float diffuseIntensity = max(dot(dirToLight, normal), 0) * attenuation;
    vec3 diffuseColor = diffuseIntensity * light.color * diffuseTexture;

    //specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), material.shininess) * attenuation;
    vec3 specularColor = specularIntensity * light.color * specularTexture;

    float shadowStrength = 0.85;
    return isLightBox ?
    vec3(0,0,0) :
    (1.0 - shadow * shadowStrength) * (diffuseColor + specularColor);
}

vec3 getLightDirectionEffect(DirectionalLight light, vec3 normal, vec3 dirToCamara, float shadow) {
    vec3 diffuseTexture, specularTexture;
    if (isTextureOn){
        if(isFlowing){
            diffuseTexture = texture(material.diffuse, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
            specularTexture = texture(material.specular, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
        }else{
            diffuseTexture = texture(material.diffuse, TexCoord).rgb;
            specularTexture = texture(material.specular, TexCoord).rgb;
        }
    } else {
        diffuseTexture = vec3(1);
        specularTexture = vec3(1);
    }
    vec3 dirToLight = normalize(light.pos - FragPos);
    //due to unreal light reflex, using Blinn-Phong
    vec3 halfwayDir = normalize(dirToLight+dirToCamara);
    // diffuse: max(dot(L,N),0)
    float diffuseIntensity = max(dot(light.lightDir, normal), 0);
    vec3 diffuseColor = diffuseIntensity * light.color * diffuseTexture;

    // specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
    vec3 specularColor = specularIntensity * light.color * specularTexture;

    float shadowStrength = 0.85;
    return isLightBox ?
    vec3(0,0,0) :
    (1.0 - shadow * shadowStrength) * (diffuseColor + specularColor);
}

vec3 getLightSpotEffect(SpotLight light, vec3 normal, vec3 dirToCamara, float shadow) {
    vec3 diffuseTexture, specularTexture;
    if (isTextureOn){
        if(isFlowing){
            diffuseTexture = texture(material.diffuse, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
            specularTexture = texture(material.specular, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
        }else{
            diffuseTexture = texture(material.diffuse, TexCoord).rgb;
            specularTexture = texture(material.specular, TexCoord).rgb;
        }
    } else {
        diffuseTexture = vec3(1);
        specularTexture = vec3(1);
    }
    vec3 dirToLight = normalize(light.pos - FragPos);
    //due to unreal light reflex, using Blinn-Phong
    vec3 halfwayDir = normalize(dirToLight+dirToCamara);
    // attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    float spotRatio;
    float cosTheta = dot(dirToLight, -1 * light.lightDir);
    if (cosTheta > light.innerCosPhy) {
        spotRatio = 1.0f;
    } else if (cosTheta > light.outerCosPhy) {
        spotRatio = 1.0f - (cosTheta - light.innerCosPhy) / (light.outerCosPhy - light.innerCosPhy);
        spotRatio = pow(spotRatio, 2);
    } else {
        spotRatio = 0.0f;
        shadow = 0.0f;
    }

    // diffuse
    float diffuseIntensity = max(dot(dirToLight, normal), 0) * attenuation * spotRatio;
    vec3 diffuseColor = diffuseIntensity * light.color * diffuseTexture;

    // specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), material.shininess) * attenuation * spotRatio;
    vec3 specularColor = specularIntensity * light.color * specularTexture;

    float shadowStrength = 0.85;
    return isLightBox ?
    vec3(0,0,0) :
    (1.0 - shadow * shadowStrength) * (diffuseColor + specularColor);
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
//    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    //PCF
    float shadow = 0.0;
    vec2 texelSize = (1.0 / textureSize(shadowMap, 0))*0.6f;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    // no shadow out of frustum in light view
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}

vec3 getAmbientEffect() {
    vec3 diffuseTexture, specularTexture;
    if (isTextureOn){
        if(isFlowing){
            diffuseTexture = texture(material.diffuse, vec2(TexCoord.x,TexCoord.y+timeValueIncrement)).rgb;
        }else{
            diffuseTexture = texture(material.diffuse, TexCoord).rgb;
        }
    }
    vec3 ambient = ambientColor * diffuseTexture;
    vec3 result = ambient * material.ambient;
    return isLightBox ? diffuseTexture : result;
}


// for debug
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;// Back to NDC
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}


void main(){
    vec3 lightEffect = vec3(0.0, 0.0, 0.0);
    vec3 normal = normalize(Normal);

    // shadow
    vec3 lightDir = normalize(pointLight.pos - FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = calculateShadow(FragPosLightSpace, bias);
    if (!isShadowOn){
        shadow = 0.0f;
    }

    // light
    vec3 dirToCamara = normalize(cameraPos - FragPos);
    lightEffect += getPointLightEffect(pointLight, normal, dirToCamara, shadow);
    lightEffect += getLightDirectionEffect(directionalLight, normal, dirToCamara, shadow);
    lightEffect += getLightSpotEffect(spotLight, normal, dirToCamara, shadow);
    lightEffect += getAmbientEffect();

    vec3 hsvEmissionMap = rgb2hsv(texture(emissionMap, TexCoord).rgb);
    if (isGlowingOn) {
        lightEffect += hsv2rgb(vec3(timeValuePeriod, hsvEmissionMap.y, hsvEmissionMap.z));
    }

    FragColor = vec4(lightEffect, 1.0);
}