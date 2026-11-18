#pragma once

namespace rtype::client {

/** @brief What the butler says in the panel at the bottom of the screen: two
 * lines, each a constant text. */
struct ButlerLine {
  const char* first{};
  const char* second{};
};

}  // namespace rtype::client
