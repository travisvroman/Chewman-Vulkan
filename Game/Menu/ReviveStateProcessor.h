// Chewman Vulkan game
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under the MIT License
#pragma once
#include "Game/StateProcessor.h"
#include "Game/Controls/IEventHandler.h"
#include <memory>

namespace Chewman
{

class ControlDocument;

class ReviveStateProcessor : public StateProcessor, public IEventHandler
{
public:
    ReviveStateProcessor();
    ~ReviveStateProcessor() override;

    GameState update(float deltaTime) override;
    void processInput(const SDL_Event& event) override;

    void show() override;
    void hide() override;

    bool isOverlapping() override;

    // IEventHandler
    void processEvent(Control* control, EventType type, int x, int y) override;

private:
    std::unique_ptr<ControlDocument> _document;
    bool _videoAdsLaunched = false;
};

} // namespace Chewman