#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "Expulsion.hpp"

namespace rtype::server {

/** @brief Which players PlayerExpeller expels, and the reason they are told. */
struct ExpulsionRule {
  /** @brief Whether the player must go; must not be empty. */
  std::function<bool(const ConnectedPlayer&)> chosen;
  network::ExpulsionReason reason{network::ExpulsionReason::ByTheHost};
};

}  // namespace rtype::server
