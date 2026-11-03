#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How a leaderboard stands: asked and waiting; answered; given up on,
 * after asking for a while; no session to ask through; the source out of
 * reach. */
enum class LeaderboardPhase : std::uint8_t {
  Asking,
  Answered,
  NoAnswer,
  NotOnServer,
  OutOfReach,
};

}  // namespace rtype::client
