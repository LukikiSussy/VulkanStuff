#version 450

layout(push_constant) uniform Push {
    vec2 offset;
    vec2 mousePos;
    vec2 resolution;
    float scale;
    float time;
} push;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D myTexture;

float sdSphere(vec3 p, float r) {
    return length(p) - r; // distance to a sphere
}

float map(vec3 p) {
    vec3 spherePos1 = vec3(sin((push.time)), 0, cos(push.time)); // center of the sphere
    float sphere1 = sdSphere(p - spherePos1, 1.0);

    vec3 spherePos2 = vec3(sin((push.time + 3.14)), 0, cos(push.time + 3.14)); // center of the sphere
    float sphere2 = sdSphere(p - spherePos2, 1.0);

    return min(sphere1, sphere2); // distance to the sphere
}

void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0 - push.resolution) / push.resolution.y;

    // Initialization
    vec3 ro = vec3(0, 0, -3);         // ray origin
    vec3 rd = normalize(vec3(uv, 1)); // ray direction
    vec3 col = vec3(0);               // final pixel color

    float t = 0.; // total distance travelled

    int i;
    // Raymarching
    for (i = 0; i < 80; i++) {
        vec3 p = ro + rd * t;     // position along the ray

        float d = map(p);         // current distance to the scene

        t += d;                   // "march" the ray

        if (d < .001) break;      // early stop if close enough
        if (t > 100.) break;      // early stop if too far
    }

    // Coloring
    col = vec3(t * .2);           // color based on distance

    float dist = min(t * .2, 1);

    outColor = texture(myTexture, vec2(dist, 0));
}