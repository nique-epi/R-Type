#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief What the supervisor does with a player whose request is valid. */
enum class PlaceKind : std::uint8_t {
  /** @brief Send it to the game that has a place: see GamePlace::port. */
  Redirect,
  /** @brief A game that has a place is starting: answer nothing yet. */
  Wait,
  /** @brief A new game must run for it, and one more may: start one. */
  Open,
  /** @brief The player gets no place: refuse it, see GamePlace::refusal. */
  Refuse,
};

}  // namespace rtype::server
