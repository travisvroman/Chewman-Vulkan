// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under CC BY 4.0
#pragma once
#include "Libs.h"
#include "CameraSettings.h"
#include "SceneNode.h"

namespace SVE
{

class CameraNode : public SceneNode
{
public:
    explicit CameraNode(CameraSettings cameraSettings);
    CameraNode();

    void setNearFarPlane(float near, float far);
    void setFOV(float fov);
    void setAspectRatio(float aspectRatio);

    const glm::mat4& getProjectionMatrix();

    void setLookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);

    UniformData fillUniformData();
private:
    void createMatrix();

private:
    CameraSettings _cameraSettings;
    glm::mat4 _projection;
};

} // namespace SVE