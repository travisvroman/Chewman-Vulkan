// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under the MIT License
#include <glm/gtc/matrix_transform.hpp>
#include "SceneManager.h"
#include "Engine.h"
#include "LightManager.h"
#include "LightNode.h"
#include "ShadowMap.h"
#include "Skybox.h"
#include "Water.h"

namespace SVE
{

SceneManager::SceneManager()
    : _root(std::make_shared<SceneNode>("root"))
{
}

SceneManager::~SceneManager() = default;

std::shared_ptr<SceneNode> SceneManager::getRootNode()
{
    return _root;
}

std::shared_ptr<CameraNode> SceneManager::createMainCamera()
{
    _mainCamera = std::make_shared<CameraNode>();
    _root->attachSceneNode(_mainCamera);

    return _mainCamera;
}

std::shared_ptr<CameraNode> SceneManager::getMainCamera()
{
    return _mainCamera;
}

void SceneManager::setMainCamera(std::shared_ptr<CameraNode> cameraEntity)
{
    _mainCamera = cameraEntity;
}

std::shared_ptr<LightNode> SceneManager::createLight(LightSettings lightSettings)
{
    auto lightNode = std::make_shared<LightNode>(lightSettings);
    _root->attachSceneNode(lightNode);

    return lightNode;
}

LightManager* SceneManager::getLightManager()
{
    if (!_lightManager)
        _lightManager = std::make_unique<LightManager>(Engine::getInstance()->getEngineSettings().useCascadeShadowMap);
    return _lightManager.get();
}

std::shared_ptr<SceneNode> SceneManager::createSceneNode(std::string name)
{
    return std::make_shared<SceneNode>(name);
}

void SceneManager::setSkybox(const std::string& materialName)
{
    _skybox = std::make_shared<Skybox>(materialName);
}

void SceneManager::setSkybox(std::shared_ptr<Skybox> skybox)
{
    _skybox = std::move(skybox);
}

std::shared_ptr<Skybox> SceneManager::getSkybox()
{
    return _skybox;
}

std::shared_ptr<Water> SceneManager::createWater(float height)
{
    _water = std::make_shared<Water>(height);
    return _water;
}

std::shared_ptr<Water> SceneManager::getWater()
{
    return _water;
}

} // namespace SVE