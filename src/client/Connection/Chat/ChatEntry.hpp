#pragma once

#include <cstdint>
#include <string>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A line of the chat as the client keeps it: who wrote it, the
 * nickname the waiting room gave that player when the line arrived, the text,
 * and how long ago it arrived. */
struct ChatEntry {
  /** @brief FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS for a pilot, CLIENT_WRITER
   * for a line the client writes itself. */
  std::uint8_t playerNumber{};
  /** @brief Empty when the waiting room listed no player of that number. */
  std::string nickname;
  std::string text;
  engine::Duration age{};

  bool operator==(const ChatEntry& other) const = default;
};

}  // namespace rtype::client
