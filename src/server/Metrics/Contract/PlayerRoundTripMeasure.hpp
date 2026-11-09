#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "RoundTripFigures.hpp"

namespace rtype::server {

/** @brief The round trip of one player of the server, and how its inputs
 * kept pace with the ticks, as the metrics report them. */
struct PlayerRoundTripMeasure {
  /** @brief The number of the game of the player: NUMBER_OF_THE_GAME in a
   * game process, which runs one game. */
  std::size_t gameNumber{0};
  std::uint8_t playerNumber{0};
  std::string nickname;
  network::RoundTripFigures figures;
  /** @brief The ticks that found no input of the player due, and those that
   * applied two at once to catch up, since its first input (see InputPace). */
  std::uint64_t ticksWithoutInput{0};
  std::uint64_t catchUps{0};
};

}  // namespace rtype::server
