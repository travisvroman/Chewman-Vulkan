// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under CC BY 4.0

#pragma once

#include <cstdint>

namespace SVE
{

struct EngineSettings
{
    enum class PresentMode : uint8_t
    {
        FIFO = 0,
        Mailbox,
        Immediate,
        BestAvailable
    };

    const char* applicationName = "VulkanApp";
    int gpuIndex = BEST_GPU_AVAILABLE;
    PresentMode presentMode = PresentMode::BestAvailable;
    int MSAALevel = BEST_MSAA_AVAILABLE;
    bool useValidation = false;

    static const int BEST_GPU_AVAILABLE;
    static const int BEST_MSAA_AVAILABLE;
};

} // namespace SVE