#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "RoundTripFigures.hpp"

namespace rtype::server {

/** @brief The round trip of one player of the server, as the metrics report
 * it. */
struct PlayerRoundTripMeasure {
  /** @brief 1 for the first game Games::all() lists, then one more for each.
   */
  std::size_t gameNumber{0};
  std::uint8_t playerNumber{0};
  std::string nickname;
  network::RoundTripFigures figures;
};

}  // namespace rtype::server
