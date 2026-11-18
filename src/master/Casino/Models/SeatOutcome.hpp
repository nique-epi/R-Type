#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief What a request for a seat of the casino did. */
enum class SeatOutcome : std::uint8_t {
  /** @brief The account sat down. */
  Taken,
  /** @brief The account was seated already: its seat lasts longer. */
  Renewed,
  /** @brief Every seat of the room is taken by other accounts. */
  RoomFull,
};

}  // namespace rtype::master
