// VSE (Vulkan Simple Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under the MIT License
#pragma once
#include <cstdint>

template<typename T>
uint8_t toInt(T enumVal)
{
    return static_cast<uint8_t>(enumVal);
}
