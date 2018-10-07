{
    "name": "phongShadowFragmentShader",
    "filename": "glsl/phongShadowMap.frag.spv",
    "shaderType": "FragmentShader",
    "samplerNamesList": [
        "diffuseTex",
        "shadowTex"
    ],
    "uniformList": [
        { "uniformType": "LightViewProjection" },
        { "uniformType": "LightPosition" },
        { "uniformType": "LightColor" },
        { "uniformType": "CameraPosition" },
        { "uniformType": "LightAmbient" },
        { "uniformType": "LightDiffuse" },
        { "uniformType": "LightSpecular" },
        { "uniformType": "LightShininess" }
    ]
}