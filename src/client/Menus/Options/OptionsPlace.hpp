#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the options are opened over. */
enum class OptionsPlace : std::uint8_t {
  OverTitle,
  OverGame,
};

}  // namespace rtype::client
