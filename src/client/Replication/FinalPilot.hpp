#pragma once

#include <cstdint>
#include <string>

namespace rtype::client {

/** @brief A player at the end of a game: its number, the nickname it joined
 * with (empty when the waiting room did not list it), its score, whether it
 * is this client, and its rank, 1 for the best. */
struct FinalPilot {
  std::uint8_t playerNumber{};
  std::string nickname;
  std::uint32_t score{};
  bool isLocalPlayer = false;
  int rank{};
};

}  // namespace rtype::client
