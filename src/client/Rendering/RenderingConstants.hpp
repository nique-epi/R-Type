#pragma once

#include <cstddef>
#include <string_view>
#include "Layer.hpp"

namespace rtype::client {

constexpr std::size_t LAYER_COUNT =
    static_cast<std::size_t>(game::Layer::Interface) + 1;

constexpr float HALF = 0.5F;
constexpr std::string_view LOG_MODULE_NAME = "Rendering";

}  // namespace rtype::client
