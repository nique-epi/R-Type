#pragma once

#include <cstdint>

namespace rtype::tools {

/** @brief What flies the bots of a launch. */
enum class BotPilotKind : std::uint8_t {
  Random,
  Rules,
};

}  // namespace rtype::tools
