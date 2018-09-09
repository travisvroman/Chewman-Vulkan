// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under CC BY 4.0
#pragma once
#include <memory>
#include <list>
#include "Libs.h"
#include "Entity.h"

namespace SVE
{

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
public:
    explicit SceneNode(std::string name);
    SceneNode();

    const std::string& getName() const;

    void setParent(std::shared_ptr<SceneNode> parent);
    std::shared_ptr<SceneNode> getParent() const;

    void attachEntity(std::shared_ptr<Entity> entity);
    void detachEntity(std::shared_ptr<Entity> entity);
    const std::list<std::shared_ptr<Entity>>& getAttachedEntities() const;

    void attachSceneNode(std::shared_ptr<SceneNode> sceneNode);
    void detachSceneNode(std::shared_ptr<SceneNode> sceneNode);
    const std::list<std::shared_ptr<SceneNode>>& getChildren() const;

    const glm::mat4& getNodeTransformation() const;
    void setNodeTransformation(glm::mat4 transform);

    glm::mat4 getTotalTransformation() const;

private:
    std::string _name;
    std::shared_ptr<SceneNode> _parent;
    std::list<std::shared_ptr<Entity>> _entityList;
    std::list<std::shared_ptr<SceneNode>> _sceneNodeList;

    glm::mat4 _transformation = glm::mat4(1);
};

} // namespace SVE