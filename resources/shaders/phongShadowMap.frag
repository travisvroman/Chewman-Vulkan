#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D diffuseTex;
layout(set = 1, binding = 1) uniform sampler2D shadowTex;
layout(set = 1, binding = 2) uniform UBO
{
    vec4 lightPos;
	vec4 lightColor;
	vec4 cameraPos;
	float ambientStrength;
	float diffuseStrength;
    float specularStrength;
    float shininess;
} ubo;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec4 fragLightSpacePos;

layout(location = 0) out vec4 outColor;

float textureProj(vec4 P, vec2 off)
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 )
	{
		float dist = texture( shadowTex, shadowCoord.st + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z )
		{
			shadow = ubo.ambientStrength;
		}
	}
	return shadow;
}

float computeShadowFactor(vec4 lightSpacePos)
{
   // Convert light space position to NDC (normalized device coordinates)
   vec3 lightSpaceReal = lightSpacePos.xyz /= lightSpacePos.w;

   // If the fragment is outside the light's projection then it is outside
   // the light's influence, which means it is in the shadow (notice that
   // such sample would be outside the shadow map image)
   if (abs(lightSpaceReal.x) > 1.0 ||
       abs(lightSpaceReal.y) > 1.0 ||
       abs(lightSpaceReal.z) > 1.0)
      return 0.0;

   // Translate from NDC to shadow map space (Vulkan's Z is already in [0..1])
   vec2 shadowMapCoord = lightSpaceReal.xy * 0.5 + 0.5;

   // Check if the sample is in the light or in the shadow
   if (lightSpaceReal.z > texture(shadowTex, shadowMapCoord.xy).x)
      return 0.0; // In the shadow

   // In the light
   return 1.0;
}

void main() {
    // ambient
    vec3 ambient = ubo.ambientStrength * ubo.lightColor.rgb;

    // diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(ubo.lightPos.xyz - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * ubo.diffuseStrength * ubo.lightColor.rgb;

    // specular
    vec3 viewDir = normalize(ubo.cameraPos.xyz - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), ubo.shininess);
    vec3 specular = ubo.specularStrength * spec * ubo.lightColor.rgb;

    vec3 result = (ambient + diffuse + specular) * fragColor * texture(diffuseTex, fragTexCoord).rgb * textureProj(fragLightSpacePos / fragLightSpacePos.w, vec2(0.0)); // * computeShadowFactor(fragLightSpacePos);
    outColor = vec4(result, 1.0);
}