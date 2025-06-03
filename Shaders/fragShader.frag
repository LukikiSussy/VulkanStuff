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
    return length(p) - r;
}

vec3 rot3D(vec3 p, vec3 axis, float angle) {
    return mix(dot(axis, p) * axis, p, cos(angle)) + cross(axis, p) * sin(angle);
}

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float smin (float a, float b, float k) {
    float h = max(k - abs(a-b), 0.0) / k;
    return min(a, b) - h*h*h*k*(1.0/6.0);
}

//float map(vec3 p) {
//    vec3 spherePos = vec3(tan((push.time)), 0, 0);
//    float sphere = sdSphere(p - spherePos, 1.0);
//
//    vec3 q = p;
//    q.xyz = rot3D(q.xyz, vec3(0, 0, 1), push.time * 2);
//
//    float box = sdBox(q * 4, vec3(0.75)) / 4;
//
//    float ground = -p.y + 0.6;
//
//    return min(smin(sphere, box, 2), ground);
//}

float map(vec3 p) {
    vec3 w = p;
    float m = dot(w,w);

    vec4 trap = vec4(abs(w),m);
	float dz = 1.0;

    for( int i=0; i<4; i++ ) {
        dz = 8.0*pow(m,3.5)*dz + 1.0;
      
        // z = z^8+c
        float r = length(w);
        float b = 8.0*acos( w.y/r);
        float a = 8.0*atan( w.x, w.z );
        w = p + pow(r,8.0) * vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );

        trap = min( trap, vec4(abs(w),m) );
        m = dot(w,w);
		if( m > 256.0 )
            break;
    }

    return 0.25*log(m)*sqrt(m)/dz;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0 - push.resolution) / push.resolution.y;
    vec2 m = push.mousePos;

    vec3 ro = vec3(0, 0, -3) * push.scale;         
    vec3 rd = normalize(vec3(uv, 1));
    vec3 col = vec3(0);

    float t = 0.;

    ro.yz *= rot2D(-m.y * 4);
    rd.yz *= rot2D(-m.y * 4);

    ro.xz *= rot2D(-m.x * 6);
    rd.xz *= rot2D(-m.x * 6);

    int i;
    // Raymarching
    for (i = 0; i < 80; i++) {
        vec3 p = ro + rd * t;

        float d = map(p);

        t += d;

        if (d < .001) break;
        if (t > 100.) break;
    }

    // Coloring
    col = vec3(t * .2);

    float dist = min(t * .2, 1);

    outColor = texture(myTexture, vec2(dist * (1/push.scale), 0));
}