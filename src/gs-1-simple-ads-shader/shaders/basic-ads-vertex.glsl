#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvCoord;
layout(location = 3) in mat4 mvpMatrix;

uniform mat4 projectionMatrix;
uniform vec4 color;

out VS_OUT
{
    vec4 color;
} vs_out;

vec3 ADSLightModel(in vec3 normal, in vec3 position, in vec3 materialAmbientColor)
{
    const vec3 lightPosition = vec3(5.0, 5.5, 5.0);

    //stubbed light properties
    const vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
    const vec3 lightDiffuse = vec3(1.0, 1.0, 1.0);
    const vec3 lightSpecular = vec3(1.0, 1.0, 1.0);

    //stubbed material properties
    const vec3 materialAmbient = materialAmbientColor;
    const vec3 materialDiffuse = vec3(1.0, 0.5, 0.0);
    const vec3 materialSpecular = vec3(0.6, 0.6, 0.6);
    const float materialShininess = 80.0;

    //normal, light, view, and light reflection vectors
    vec3 normalVector = normalize(normal);
    vec3 lightVector = normalize(lightPosition - position);
    vec3 viewVector = normalize(vec3(0.0, 0.0, 0.0) - position);
    vec3 reflectionVector = reflect(vec3(0.0, 0.0, 0.0) - lightVector, normalVector);

    //ambient light computation
    vec3 ambient = materialAmbient * lightAmbient;

    //diffuse light computation
    vec3 diffuse = max(0.0, dot(lightVector, normalVector)) * materialDiffuse * lightDiffuse;

    //optionally you can add a diffuse attenuation term at this point

    //specular light computation
    vec3 specular = vec3(0.0, 0.0, 0.0);
    if (dot(lightVector, viewVector) > 0.0) {
        specular = pow(
            max(0.0, dot(viewVector, reflectionVector)),
            materialShininess
        ) * materialSpecular
        * lightSpecular;
    }

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

void main(void)
{
    vec4 preventUnusedOptimizingOut = vec4(normal, 0) + vec4(uvCoord, 0, 0) - vec4(normal, 0) - vec4(uvCoord, 0, 0);

    /* vs_out.color = color; */
    vs_out.color = vec4(ADSLightModel(normal, position, color.rgb), 1.0);

    gl_Position = projectionMatrix * mvpMatrix * vec4(position, 1);
    gl_Position += preventUnusedOptimizingOut;
}
