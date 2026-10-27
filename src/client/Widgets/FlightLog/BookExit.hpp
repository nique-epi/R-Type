#pragma once

#include <optional>
#include "EndChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How the book leaves: none while the player has not chosen, else the
 * choice and how long ago the book started to leave. */
struct BookExit {
  std::optional<EndChoice> choice;
  engine::Duration age{};
};

}  // namespace rtype::client
