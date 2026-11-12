#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "Bounds.hpp"
#include "ComponentRegistry.hpp"

namespace rtype::client {

/**
 * @returns How far left the scroll carries an obstacle from the tick the
 * client draws, @p shownTick, to the tick the server is at, @p serverTick:
 * @p scrollSpeed, in logical units a second, times the time between them; 0
 * while the clock of the server is unknown or behind the tick drawn. The server
 * plays a command a trip later still, which this leaves out: 3 units for a
 * round trip of 50 ms at 120 units a second.
 */
[[nodiscard]] float scrollLeadOf(float scrollSpeed,
                                 std::optional<double> serverTick,
                                 std::uint32_t shownTick);

/**
 * @brief Fills @p boxes with the box of every blocking obstacle of
 * @p components, as it is drawn, moved @p lead logical units to the left:
 * where the server holds it when it plays the command predicted. Not during a
 * forEach of @p components.
 */
void collectBlockingObstacles(engine::ComponentRegistry& components, float lead,
                              std::vector<engine::Bounds>& boxes);

}  // namespace rtype::client
