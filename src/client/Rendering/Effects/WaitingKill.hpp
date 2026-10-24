#pragma once

#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct WaitingKill {
  game::Position place{};
  bool big = false;
  engine::Duration age{};
};

}  // namespace rtype::client
