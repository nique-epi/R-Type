#pragma once

namespace rtype::client {

/** @brief A row of the flame of a ship: how many pixels under the top of its
 * sprite, and the share of the length of the flame it reaches. */
struct IntroExhaustRow {
  int row{};
  float share{};
};

}  // namespace rtype::client
