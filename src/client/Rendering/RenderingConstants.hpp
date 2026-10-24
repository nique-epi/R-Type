#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "Layer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

constexpr std::size_t LAYER_COUNT =
    static_cast<std::size_t>(game::Layer::Interface) + 1;

constexpr float HALF = 0.5F;
constexpr std::string_view LOG_MODULE_NAME = "Rendering";

/** @brief An invulnerable ship shows its sprite, then its dithered picture,
 * INVULNERABILITY_BLINK_STEP, 4 frames, each. */
constexpr engine::Duration INVULNERABILITY_BLINK_STEP =
    std::chrono::milliseconds(67);

}  // namespace rtype::client
