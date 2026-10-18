#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "EntityType.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief An entity of the sprite showcase, and where its center stands, as
 * shares of the playfield from 0 (left or top edge) to 1 (right or bottom
 * edge). */
struct ShowcaseEntity {
  game::EntityType type{};
  float shareOfPlayfieldWidth{};
  float shareOfPlayfieldHeight{};
};

/** @brief What a step of the showcase loop plays. */
enum class ShowcaseCue : std::uint8_t {
  /** @brief Both missiles flash, as if just fired. */
  Shots,
  /** @brief An impact bursts on the left side of the first Bydo, which
   * flashes. */
  Hit,
  /** @brief The first Bydo explodes and disappears. */
  BydoExplodes,
  /** @brief The ship explodes and disappears. */
  ShipExplodes,
  /** @brief The ship and the first Bydo come back. */
  Return,
};

/** @brief A step of the showcase loop: when it plays, from the start of the
 * loop, and what it plays. */
struct ShowcaseStep {
  engine::Duration at{};
  ShowcaseCue cue{};
};

/** @brief Height of the showcase row, as a share of the playfield height: its
 * middle. */
constexpr float SHOWCASE_ROW_SHARE_OF_HEIGHT = 0.5F;

/** @brief Heights of the two Bydos behind the first one, above and below the
 * row, as shares of the playfield height. */
constexpr float SHOWCASE_UPPER_SHARE_OF_HEIGHT = 0.38F;
constexpr float SHOWCASE_LOWER_SHARE_OF_HEIGHT = 0.62F;

/**
 * @brief The showcase, from left to right: a player ship, a player missile, an
 * enemy missile, and three Bydos, the first in the row and two behind it,
 * above and below, so their pulses can be seen out of step.
 */
constexpr std::array SPRITE_SHOWCASE{
    ShowcaseEntity{.type = game::EntityType::Player,
                   .shareOfPlayfieldWidth = 0.2F,
                   .shareOfPlayfieldHeight = SHOWCASE_ROW_SHARE_OF_HEIGHT},
    ShowcaseEntity{.type = game::EntityType::PlayerMissile,
                   .shareOfPlayfieldWidth = 0.35F,
                   .shareOfPlayfieldHeight = SHOWCASE_ROW_SHARE_OF_HEIGHT},
    ShowcaseEntity{.type = game::EntityType::EnemyMissile,
                   .shareOfPlayfieldWidth = 0.65F,
                   .shareOfPlayfieldHeight = SHOWCASE_ROW_SHARE_OF_HEIGHT},
    ShowcaseEntity{.type = game::EntityType::Enemy,
                   .shareOfPlayfieldWidth = 0.8F,
                   .shareOfPlayfieldHeight = SHOWCASE_ROW_SHARE_OF_HEIGHT},
    ShowcaseEntity{.type = game::EntityType::Enemy,
                   .shareOfPlayfieldWidth = 0.86F,
                   .shareOfPlayfieldHeight = SHOWCASE_UPPER_SHARE_OF_HEIGHT},
    ShowcaseEntity{.type = game::EntityType::Enemy,
                   .shareOfPlayfieldWidth = 0.86F,
                   .shareOfPlayfieldHeight = SHOWCASE_LOWER_SHARE_OF_HEIGHT},
};

/** @brief Places in SPRITE_SHOWCASE of the entities the loop acts on. */
constexpr std::size_t SHOWCASE_SHIP = 0;
constexpr std::size_t SHOWCASE_PLAYER_MISSILE = 1;
constexpr std::size_t SHOWCASE_ENEMY_MISSILE = 2;
constexpr std::size_t SHOWCASE_FIRST_BYDO = 3;

/** @brief Network id of the first showcase entity; the next ones follow, in
 * the order of SPRITE_SHOWCASE, as the server numbers the entities it
 * creates. */
constexpr std::uint32_t FIRST_SHOWCASE_NETWORK_ID = 1;

/** @brief Length of the showcase loop, after which its steps play again. */
constexpr engine::Duration SHOWCASE_LOOP = std::chrono::milliseconds(4000);

/** @brief When each step of the loop plays, from the start of the loop. */
constexpr engine::Duration SHOWCASE_SHOTS_AT = std::chrono::milliseconds(200);
constexpr engine::Duration SHOWCASE_FIRST_HIT_AT =
    std::chrono::milliseconds(700);
constexpr engine::Duration SHOWCASE_SECOND_HIT_AT =
    std::chrono::milliseconds(1300);
constexpr engine::Duration SHOWCASE_BYDO_EXPLODES_AT =
    std::chrono::milliseconds(2000);
constexpr engine::Duration SHOWCASE_SHIP_EXPLODES_AT =
    std::chrono::milliseconds(2600);
constexpr engine::Duration SHOWCASE_RETURN_AT = std::chrono::milliseconds(3600);

/** @brief The steps of the loop, in the order they play. */
constexpr std::array SHOWCASE_TIMELINE{
    ShowcaseStep{.at = SHOWCASE_SHOTS_AT, .cue = ShowcaseCue::Shots},
    ShowcaseStep{.at = SHOWCASE_FIRST_HIT_AT, .cue = ShowcaseCue::Hit},
    ShowcaseStep{.at = SHOWCASE_SECOND_HIT_AT, .cue = ShowcaseCue::Hit},
    ShowcaseStep{.at = SHOWCASE_BYDO_EXPLODES_AT,
                 .cue = ShowcaseCue::BydoExplodes},
    ShowcaseStep{.at = SHOWCASE_SHIP_EXPLODES_AT,
                 .cue = ShowcaseCue::ShipExplodes},
    ShowcaseStep{.at = SHOWCASE_RETURN_AT, .cue = ShowcaseCue::Return},
};

}  // namespace rtype::client
