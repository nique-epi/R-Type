#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief A pose of the butler: a row of his sheet, from the top. */
enum class ButlerPose : std::uint8_t {
  Idle,
  Greeting,
  Bowing,
};

}  // namespace rtype::client
