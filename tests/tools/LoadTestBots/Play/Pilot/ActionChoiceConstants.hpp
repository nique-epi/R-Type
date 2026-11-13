#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include "PlayerAction.hpp"

namespace rtype::tools {

/** @brief The ways to hold a direction: none, four, then the diagonals. Each
 * is held with or without the fire. */
constexpr std::array<std::array<std::optional<client::PlayerAction>, 2>, 9>
    DIRECTION_CHOICES{
        {{std::nullopt, std::nullopt},
         {client::PlayerAction::Up, std::nullopt},
         {client::PlayerAction::Down, std::nullopt},
         {client::PlayerAction::Left, std::nullopt},
         {client::PlayerAction::Right, std::nullopt},
         {client::PlayerAction::Up, client::PlayerAction::Left},
         {client::PlayerAction::Up, client::PlayerAction::Right},
         {client::PlayerAction::Down, client::PlayerAction::Left},
         {client::PlayerAction::Down, client::PlayerAction::Right}}};

constexpr std::size_t ACTION_CHOICE_COUNT = DIRECTION_CHOICES.size() * 2;

}  // namespace rtype::tools
