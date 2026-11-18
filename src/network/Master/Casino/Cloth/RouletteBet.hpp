#pragma once

#include <cstdint>
#include "RouletteBetKind.hpp"

namespace rtype::network {

/**
 * @brief One stack of chips on the cloth: its kind, the first number it
 * covers, and its chips.
 */
struct RouletteBet {
  RouletteBetKind kind{RouletteBetKind::Straight};
  std::uint8_t firstNumber{};
  std::uint32_t amount{};

  bool operator==(const RouletteBet&) const = default;
};

}  // namespace rtype::network
