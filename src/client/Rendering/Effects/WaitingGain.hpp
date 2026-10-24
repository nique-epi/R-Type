#pragma once

#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

struct WaitingGain {
  int pilotColor{};
  std::uint32_t points{};
  engine::Duration age{};
};

}  // namespace rtype::client
