#pragma once

#include "SpriteFrame.hpp"

/** @brief Stands for the frame of an entity or a type without a sprite: it
 * lies nowhere on a sheet. */
constexpr rtype::game::SpriteFrame NO_FRAME{
    .left = 0, .top = 0, .width = 0, .height = 0};
