// Copyright (c) 2018-2019, Igor Barinov
// This file should be included after UBO declaration

/////// HELPER FUNCTIONS ////////////

// for point
float shadowPointLight(vec4 lightSpacePos, vec3 offset, uint layer)
{
    vec3 fragToLight = fragPos - lightSpacePos.xyz;
    float closestDepth = texture(pointShadowTex, vec4(vec3(fragToLight.x, fragToLight.y, -fragToLight.z) + offset, layer)).r;
    float currentDepth = length(fragToLight);

    //return closestDepth / 50;
    return currentDepth < closestDepth + 0.05 ? 1.0 : 0.4;
}

float PCFShadowPointLight(vec4 lightSpacePos, uint layer)
{
    ivec2 texDim = textureSize(pointShadowTex, 0).xy;
    float scale = 1.5;
    float dx = scale * 1.0 / float(texDim.x);
    float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int range = 1;

    const vec3 sampleOffsetDirections[20] = vec3[]
    (
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );
    const int count = 20;
    const float diskRadius = 0.05;

    for(int i = 0; i < count; ++i)
    {
        shadowFactor += shadowPointLight(lightSpacePos, sampleOffsetDirections[i] * diskRadius, layer);
    }
    return shadowFactor / count;
}

// for directional
float PCFShadowSunLight()
{
    ivec2 texDim = textureSize(directShadowTex, 0).xy;
    float scale = 1.5;
    float dx = scale * 1.0 / float(texDim.x);
    float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int count = 0;
    int range = 2;
    uint closestLayer = CASCADE_NUM - 1;
    uint layer;
    for (layer = 0; layer < CASCADE_NUM; layer++)
    {
        if (abs(fragDirectLightSpacePos[layer].x) <= 1.0 &&
        abs(fragDirectLightSpacePos[layer].y) <= 1.0 &&
        abs(fragDirectLightSpacePos[layer].z) <= 1.0)
        {
            closestLayer = layer;
            break;
        }
    }

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            // Translate from NDC to shadow map space (Vulkan's Z is already in [0..1])
            vec2 offset = vec2(dx*x, dy*y);
            vec2 shadowMapCoord = fragDirectLightSpacePos[closestLayer].xy * 0.5 + 0.5;

            // Check if the sample is in the light or in the shadow
            if (fragDirectLightSpacePos[closestLayer].z > texture(directShadowTex, vec3(shadowMapCoord.xy + offset, closestLayer)).x)
            {
                shadowFactor += 0.4;
            } else {
                shadowFactor += 1.0;
            }

            count++;
        }

    }
    return shadowFactor / count;
}

/////// Light and shadow calculation ////////////

vec3 calculateLight(vec3 normal, vec3 viewDir)
{
    vec3 lightEffect = vec3(0);
    float shadow = 0;
    uint count = 0;

    if ((ubo.lightInfo.lightFlags & LI_DirectionalLight) != 0)
    {
        vec3 curLight = CalcDirLight(ubo.dirLight, normal, viewDir, ubo.materialInfo);
        shadow = PCFShadowSunLight();
        lightEffect += curLight * (shadow);
        count++;
    }

    uint shadowCount = 0;
    for (uint i = 0; i < 3; i++)
    {
        shadow = 1;
        if ((ubo.lightInfo.lightFlags & LI_PointLight[i]) != 0)
        {
            vec3 curLight = CalcPointLight(ubo.shadowPointLight[i], normal, fragPos, viewDir, ubo.materialInfo);
            if ((ubo.lightInfo.lightShadowFlags & LI_PointLight[i]) != 0)
            {
                shadow = PCFShadowPointLight(ubo.shadowPointLight[i].position, shadowCount);
                count ++;
                shadowCount ++;
            }
            lightEffect += curLight * (shadow);
        }
    }

    for (uint i = 0; i < ubo.lightInfo.lightLineNum; i++)
    {
        lightEffect += CalcLineLight(ubo.lineLight[i], normal, fragPos, viewDir, ubo.materialInfo);
    }

    for (uint i = 0; i < ubo.lightInfo.lightPointsNum; i++)
    {
        lightEffect += CalcPointLight(ubo.pointLight[i], normal, fragPos, viewDir, ubo.materialInfo);
    }

    if ((ubo.lightInfo.lightFlags & LI_SpotLight) != 0)
    lightEffect += CalcSpotLight(ubo.spotLight, normal, fragPos, viewDir, ubo.materialInfo);

    return lightEffect;
}