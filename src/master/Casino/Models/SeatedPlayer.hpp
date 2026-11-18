#pragma once

#include <cstdint>
#include <string>

namespace rtype::master {

/** @brief An account seated in a room of the casino, with its nickname. */
struct SeatedPlayer {
  std::int64_t userId{};
  std::string nickname;
};

}  // namespace rtype::master
