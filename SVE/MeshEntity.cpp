// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under CC BY 4.0

#include "MeshEntity.h"
#include "Engine.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "VulkanMesh.h"
#include "VulkanMaterial.h"
#include "Utils.h"

namespace SVE
{

MeshEntity::MeshEntity(std::string name)
    : MeshEntity(Engine::getInstance()->getMeshManager()->getMesh(name))
{

}

MeshEntity::MeshEntity(std::shared_ptr<Mesh> mesh)
    : _mesh(mesh)
    , _material(Engine::getInstance()->getMaterialManager()->getMaterial(mesh->getDefaultMaterialName()))
{
    if (_material)
    {
        setupMaterial();
    }
}

MeshEntity::~MeshEntity() = default;

void MeshEntity::setMaterial(const std::string& materialName)
{
    // TODO: Refactor this idiocy
    if (materialName == "WaterReflection")
    {
        _waterMaterial = true;
        _castShadows = false;
    }
    _material = Engine::getInstance()->getMaterialManager()->getMaterial(materialName);
    setupMaterial();
}

void MeshEntity::setCastShadows(bool castShadows)
{
    _castShadows = castShadows;
}


void MeshEntity::updateUniforms(UniformDataList uniformDataList) const
{
    UniformData newData = *uniformDataList[toInt(CommandsType::MainPass)];

    _mesh->updateUniformDataBones(newData, Engine::getInstance()->getTime());
    _material->getVulkanMaterial()->setUniformData(_materialIndex, newData);

    if (_shadowMaterial)
    {
        UniformData newShadowData = *uniformDataList[toInt(CommandsType::ShadowPass)];
        newShadowData.bones = newData.bones;
        _shadowMaterial->getVulkanMaterial()->setUniformData(_shadowMaterialIndex, newShadowData);
    }
    if (Engine::getInstance()->isWaterEnabled())
    {
        UniformData newReflectionData = *uniformDataList[toInt(CommandsType::ReflectionPass)];
        newReflectionData.bones = newData.bones;
        _material->getVulkanMaterial()->setUniformData(_reflectionMaterialIndex, newReflectionData);

        /*UniformData newRefractionData = *uniformDataList[toInt(CommandsType::ReflectionPass)];
        newRefractionData.bones = newData.bones;
        _material->getVulkanMaterial()->setUniformData(_reflectionMaterialIndex, newReflectionData);*/
    }
}

void MeshEntity::applyDrawingCommands(uint32_t bufferIndex, bool applyMaterial) const
{
    if (Engine::getInstance()->getPassType() == CommandsType::ReflectionPass)
    {
        if (_waterMaterial)
            return;
        _material->getVulkanMaterial()->applyDrawingCommands(bufferIndex, _reflectionMaterialIndex);

    } else if (Engine::getInstance()->getPassType() == CommandsType::RefractionPass)
    {
        if (_waterMaterial)
            return;

        _material->getVulkanMaterial()->applyDrawingCommands(bufferIndex, _materialIndex);
    }
    else
    {
        if (!applyMaterial)
        {
            if (_shadowMaterial && _castShadows)
                _shadowMaterial->getVulkanMaterial()->applyDrawingCommands(bufferIndex, _shadowMaterialIndex);
        }
        else
        {
            _material->getVulkanMaterial()->applyDrawingCommands(bufferIndex, _materialIndex);
        }
    }

    _mesh->getVulkanMesh()->applyDrawingCommands(bufferIndex);
}

void MeshEntity::setupMaterial()
{
    _materialIndex = _material->getVulkanMaterial()->getInstanceForEntity(this);

    if (Engine::getInstance()->isWaterEnabled())
    {
        _reflectionMaterialIndex = _material->getVulkanMaterial()->getInstanceForEntity(this, 1);
    }

    if (Engine::getInstance()->isShadowMappingEnabled())
    {
        // TODO: Get shadow materials (or their names) from shadowmap class or special function in MatManager
        if (_material->getVulkanMaterial()->isSkeletal())
            _shadowMaterial = Engine::getInstance()->getMaterialManager()->getMaterial("SimpleSkeletalDepth");
        else
            _shadowMaterial = Engine::getInstance()->getMaterialManager()->getMaterial("SimpleDepth");

        _shadowMaterialIndex = _shadowMaterial->getVulkanMaterial()->getInstanceForEntity(this);
    }
}

} // namespace SVE