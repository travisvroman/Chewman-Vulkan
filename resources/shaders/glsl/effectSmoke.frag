// Based on https://www.shadertoy.com/view/MdyGzR by ehj1

#version 450
#extension GL_ARB_separate_shader_objects : enable
#include "lighting.glsl"

layout(set = 1, binding = 0) uniform UBO
{
    vec4 cameraPos;
    MaterialInfo materialInfo;
    float width;
    float time;
} ubo;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outColorBloom;

vec3 mod289(vec3 x) 
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) 
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) 
{
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
{
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
    i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
    + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
    + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
    dot(p2,x2), dot(p3,x3) ) );
}

float normnoise(float noise) 
{
    return 0.5*(noise+1.0);
}

float clouds(vec2 uv) 
{
    //uv += vec2(ubo.time*0.05, + ubo.time*0.01);
    float time = ubo.time * 0.6;
    vec2 off1 = vec2(50.0,33.0);
    vec2 off2 = vec2(0.0, 0.0);
    vec2 off3 = vec2(-300.0, 50.0);
    vec2 off4 = vec2(-100.0, 200.0);
    vec2 off5 = vec2(400.0, -200.0);
    vec2 off6 = vec2(100.0, -1000.0);
    float scale1 = 3.0;
    float scale2 = 6.0;
    float scale3 = 12.0;
    float scale4 = 24.0;
    float scale5 = 48.0;
    float scale6 = 96.0;
    return normnoise(snoise(vec3((uv+off1)*scale1,time*0.5))*0.8 +
                     snoise(vec3((uv+off2)*scale2,time*0.4))*0.4 +
                     snoise(vec3((uv+off3)*scale3,time*0.1))*0.2 +
                     snoise(vec3((uv+off4)*scale4,time*0.7))*0.1 +
                     snoise(vec3((uv+off5)*scale5,time*0.2))*0.05 +
                     snoise(vec3((uv+off6)*scale6,time*0.3))*0.025);
}

float linearDepth(float z_b)
{
    float zNear = 0.1 ;
    float zFar = 500.0;

    return (-zFar * zNear / (z_b * (zFar - zNear) - zFar));
}

void main() 
{
    float cloudColor = clouds(fragTexCoord * 10.0);
    vec3 finalColor = vec3(cloudColor/5.0, cloudColor / 3.5, cloudColor/4.5);
    finalColor = finalColor * 0.5;

    vec3 cameraDir = fragPos - ubo.cameraPos.xyz;
    float len1 = length(cameraDir);
    cameraDir = normalize(cameraDir);
    vec3 toWall = vec3(0, 0, 1);
    float len2 = (-ubo.cameraPos.z + 1.5) / dot(toWall, cameraDir);

    float depthDiff = (len2 - len1) * 0.5;
    //depthDiff = fragPos.x > -1.5 ? depthDiff;
    depthDiff = mix(depthDiff, 1.0, smoothstep(-1.5, -2, fragPos.x));
    depthDiff = mix(depthDiff, 1.0, smoothstep(ubo.width - 1.5, ubo.width-1.0, fragPos.x));

    outColor = vec4(finalColor, abs(depthDiff));

    outColorBloom = vec4(outColor.rgb, 0.35);
}