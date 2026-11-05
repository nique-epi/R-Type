#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief What the supervisor does with a player whose request is valid. */
enum class PlaceKind : std::uint8_t {
  /** @brief Send it to the game that has a place: see GamePlace::port. */
  Redirect,
  /** @brief A game that has a place is starting: answer nothing yet. */
  Wait,
  /** @brief No game has a place and one more may run: start one. */
  Open,
  /** @brief No game has a place and no more may run: refuse. */
  Full,
};

}  // namespace rtype::server
