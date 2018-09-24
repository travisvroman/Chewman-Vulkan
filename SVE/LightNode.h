// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under CC BY 4.0
#pragma once
#include "LightSettings.h"
#include "SceneNode.h"

namespace SVE
{

class LightNode : public SceneNode
{
public:
    explicit LightNode(LightSettings lightSettings);

    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix();

    const LightSettings& getLightSettings();
    void fillUniformData(UniformData& data, bool asViewSource);

    void setNodeTransformation(glm::mat4 transform) override;

private:
    void createViewMatrix();
    void createProjectionMatrix();

private:
    LightSettings _lightSettings;
    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;

};

} // namespace SVE