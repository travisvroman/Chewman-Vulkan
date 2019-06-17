// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under CC BY 4.0
#include <glm/gtc/matrix_transform.hpp>
#include "SceneManager.h"
#include "LightManager.h"
#include "LightNode.h"
#include "ParticleSystemEntity.h"
#include "ShadowMap.h"
#include "Skybox.h"
#include "Water.h"

namespace SVE
{

SceneManager::SceneManager()
{
    _root = std::make_shared<SceneNode>("root");
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
    auto lightNode = std::make_shared<LightNode>(lightSettings, getLightManager()->getLightCount());
    getLightManager()->setLight(lightNode, getLightManager()->getLightCount());
    _root->attachSceneNode(lightNode);

    return lightNode;
}

LightManager* SceneManager::getLightManager()
{
    if (!_lightManager)
        _lightManager = std::make_unique<LightManager>();
    return _lightManager.get();
}

std::shared_ptr<ParticleSystemEntity> SceneManager::createParticleSystem(ParticleSystemSettings particleSystemSettings)
{
    auto sceneNode = std::make_shared<SceneNode>(particleSystemSettings.name);
    sceneNode->setNodeTransformation(glm::translate(glm::mat4(1), glm::vec3(20, 5, 0)));
    _root->attachSceneNode(sceneNode);
    auto particleEntity = std::make_shared<ParticleSystemEntity>(std::move(particleSystemSettings));
    particleEntity->setRenderLast();
    sceneNode->attachEntity(particleEntity);

    _particleSystem = particleEntity;

    return particleEntity;
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

std::shared_ptr<ParticleSystemEntity> SceneManager::getParticleSystem()
{
    return _particleSystem;
}

} // namespace SVE