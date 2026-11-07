#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rtype::server {

/** @brief What ChatRelay keeps of the player on one seat. */
struct ChatSeat {
  /** @brief The lineNumber of the newest line relayed for the player; empty
   * until its first line, and again once its session closes. */
  std::optional<std::uint16_t> newestLineNumber;
  /** @brief The first tick of the relay at which the player may send its next
   * line. */
  std::size_t nextLineTick{0};
};

}  // namespace rtype::server
