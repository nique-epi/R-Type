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
/** @brief What a size is divided by to centre a picture on another. */
constexpr int HALF_DIVISOR = 2;
constexpr std::string_view RENDERING_LOGGER_NAME = "Rendering";
/** @brief What the log says before the id of an asset no texture was found
 * for. */
constexpr std::string_view MISSING_TEXTURE_WARNING = "no texture for asset id ";

/** @brief An invulnerable ship shows its sprite, then its dithered picture,
 * INVULNERABILITY_BLINK_STEP, 4 frames, each. */
constexpr engine::Duration INVULNERABILITY_BLINK_STEP =
    std::chrono::milliseconds(67);

}  // namespace rtype::client
