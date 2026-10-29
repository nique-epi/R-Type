#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief What carries a ship on top of its drift for a moment: nothing, a
 * barrel roll that slides it up or down, or a dash along its row.
 */
enum class ManeuverKind : std::uint8_t { None, Roll, Dash };

}  // namespace rtype::game
