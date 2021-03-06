// Chewman Vulkan game
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under the MIT License
#pragma once
#include "Game/StateProcessor.h"
#include "Game/Controls/IEventHandler.h"
#include "GameMap.h"

namespace Chewman
{

class GameMapProcessor;
class ProgressManager;
class ControlDocument;

class LevelStateProcessor : public StateProcessor, public IEventHandler
{
public:
    LevelStateProcessor();
    ~LevelStateProcessor() override;

    void initMap();

    GameState update(float deltaTime) override;
    void processInput(const SDL_Event& event) override;

    void show() override;
    void hide() override;

    bool isOverlapping() override;

    // IEventHandler
    void processEvent(Control* control, EventType type, int x, int y) override;

private:
    void updateHUD(float deltaTime);
    void updatePowerUps();
    void updateArrows();

private:
    ProgressManager& _progressManager;
    std::unique_ptr<GameMapProcessor> _gameMapProcessor;

    std::unique_ptr<ControlDocument> _document;
    std::shared_ptr<Control> _counterControl;
    std::shared_ptr<Control> _loadingControl;
    float _time = 0.0f;
    float _counterTime = 0.0;

    bool _useOnScreenControl = true;
    bool _reviveUsed = false;
    bool _showFPS = false;

    // As prev game map could be still in some commands, we need to finish rendering them all before release
    // TODO: Fix this in Engine so it won't destroy until all commands are finished
    std::unique_ptr<GameMapProcessor> _oldGameMap;
    uint32_t _countToRemove = 0;
};

} // namespace Chewman