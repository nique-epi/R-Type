#pragma once

#include <array>
#include "EntityType.hpp"
#include "SpriteShowcase.hpp"

namespace rtype::client {

/** @brief Height of the showcase row, as a share of the playfield height: its
 * middle. */
constexpr float SHOWCASE_ROW_SHARE_OF_HEIGHT = 0.5F;

/**
 * @brief The showcase, from left to right: a player ship, a player missile, an
 * enemy missile and a Bydo.
 */
constexpr std::array SPRITE_SHOWCASE{
    ShowcaseEntity{.type = game::EntityType::Player,
                   .shareOfPlayfieldWidth = 0.2F},
    ShowcaseEntity{.type = game::EntityType::PlayerMissile,
                   .shareOfPlayfieldWidth = 0.35F},
    ShowcaseEntity{.type = game::EntityType::EnemyMissile,
                   .shareOfPlayfieldWidth = 0.65F},
    ShowcaseEntity{.type = game::EntityType::Enemy,
                   .shareOfPlayfieldWidth = 0.8F},
};

}  // namespace rtype::client
