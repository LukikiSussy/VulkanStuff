#version 450

layout(push_constant) uniform Push {
    vec2 offset;
    vec2 mousePos;
    float screenSize;
    float scale;
} push;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D myTexture;


dvec2 complexSquare(dvec2 z) {
    return dvec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
}

float mandelbrot(vec2 uv) {
    dvec2 c = dvec2(uv / push.scale) + push.offset * push.screenSize * 0.5;
    dvec2 z = vec2(0.0);
    int i;
    int maxIterations = max(int(100.0 + log2(push.scale) * 50.0), 100);
    for (i = 0; i < maxIterations; i++) {
        z = complexSquare(z) + complexSquare(c);
        if (dot(z, z) > 4.0) break;  
    }
    return float(i) / 100.0;  
}

void main()
{
    vec2 uv = (gl_FragCoord.xy / push.screenSize) * 2.0 - 1.0;

    float funcVal = clamp(mandelbrot(uv), 0.01, 1.0);

    if(funcVal >= 0.99) {
        outColor = vec4(0, 0, 0, 1);
    }
    else {
        outColor = texture(myTexture, vec2(funcVal, funcVal));
    }
}