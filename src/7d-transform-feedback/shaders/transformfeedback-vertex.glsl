#version 460 core

layout (location = 0) in vec4 position_mass;
layout (location = 1) in vec3 velocity;
layout (location = 2) in ivec4 connection;

//Texture Buffer Object bound to the same buffer as the
//position_mass vertex attribute
layout (binding = 0) uniform samplerBuffer tex_position;

out vec4 tf_position_mass;
out vec3 tf_velocity;

uniform float timeStep = 0.07;
uniform float springConstant = 7.1;
uniform float dampingConstant = 2.8;
uniform float springRestingLength = 0.88;

const vec3 gravity = vec3(0.0, -0.08, 0.0);

void main()
{
    vec3 position = position_mass.xyz;
    float mass = position_mass.w;
    vec3 initialVelocity = velocity;
    vec3 force = gravity * mass - dampingConstant * initialVelocity;
    bool isFixed = true;

    for (int i = 0; i < 4; i++) {
        if (connection[i] != -1) {
            vec3 connectedVertex = texelFetch(tex_position, connection[i]).xyz;
            vec3 positionToConnectedVertex = connectedVertex - position;
            float connectionLength = length(positionToConnectedVertex);

            force += -springConstant
                * (springRestingLength - connectionLength)
                * normalize(connectionLength);

            isFixed = false;
        }
    }

    if (isFixed) {
        force = vec3(0.0);
    }

    vec3 acceleration = force / mass;
    vec3 displacement = initialVelocity * timeStep + 0.5 * acceleration * timeStep * timeStep;
    vec3 finalVelocity = initialVelocity + acceleration * timeStep;

    displacement = clamp(displacement, vec3(-25.0), vec3(25.0));

    tf_position_mass = vec4(position + displacement, mass);
    tf_velocity = finalVelocity;
}
