#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief The seat of one account in one room of the casino. */
struct SeatKey {
  std::uint8_t room{};
  std::int64_t userId{};
};

}  // namespace rtype::master
