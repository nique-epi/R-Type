#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/**
 * @brief The Bydo Tyrant at rest at (1616, 492), and its two beams as its
 * sheet lays them: from its eye, (-76, -52) from its center, so from
 * (1540, 440), 1920 long and 52 thick; its beam straight left in the strike
 * of attack 2, its sweep from -35 to 12 degrees over the 1200 ms of the
 * strike of attack 15.
 */
constexpr game::Position TYRANT_AT_REST{.x = 1616.0F, .y = 492.0F};
constexpr float EYE_OFFSET_X = -76.0F;
constexpr float EYE_OFFSET_Y = -52.0F;
constexpr float TYRANT_BEAM_LENGTH = 1920.0F;
constexpr float TYRANT_BEAM_THICKNESS = 52.0F;
constexpr float SWEEP_FROM_DEGREES = -35.0F;
constexpr float SWEEP_TO_DEGREES = 12.0F;
constexpr engine::Duration SWEEP_TIME = std::chrono::milliseconds(1200);
constexpr std::uint8_t BEAM_ATTACK = 2;
constexpr std::uint8_t SWEEP_ATTACK = 15;
constexpr game::ActionKey BEAM_STRIKE{.attack = BEAM_ATTACK,
                                      .phase = game::EnemyPhase::Strike};
constexpr game::ActionKey SWEEP_STRIKE{.attack = SWEEP_ATTACK,
                                       .phase = game::EnemyPhase::Strike};
constexpr engine::Duration HALF_THE_SWEEP = std::chrono::milliseconds(600);

/**
 * @brief Pixels of the canvas, 4 logical units each, their centers at
 * (4 column + 2, 4 row + 2):
 * - (250, 110), center (1002, 442): 2 units below the straight beam, in its
 *   core;
 * - (250, 103), center (1002, 414): 26 units above it, half its thickness,
 *   its rim;
 * - (250, 102), center (1002, 410): 30 units above it, past its rim;
 * - (263, 85), center (1054, 342): 0.86 unit off the sweep at -11.5 degrees,
 *   halfway through it, and 198 units off it at -35 degrees, as it starts.
 */
constexpr sf::Vector2i ON_THE_BEAM_AXIS{250, 110};
constexpr sf::Vector2i ON_THE_BEAM_RIM{250, 103};
constexpr sf::Vector2i PAST_THE_BEAM_RIM{250, 102};
constexpr sf::Vector2i ON_THE_SWEEP_HALFWAY{263, 85};

}  // namespace rtype::client::testing
