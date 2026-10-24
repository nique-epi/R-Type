#pragma once

#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

struct SeenGameEvent {
  std::uint16_t eventNumber{};
  engine::Duration age{};
};

}  // namespace rtype::client
