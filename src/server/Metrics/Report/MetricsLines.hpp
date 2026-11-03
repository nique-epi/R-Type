#pragma once

#include <string>
#include "MetricsReport.hpp"

namespace rtype::server {

/**
 * @returns The line of the journal that says how long the ticks of @p report
 * lasted against the budget of a tick, and how many games, players and entities
 * the server holds.
 */
[[nodiscard]] std::string ticksLineOf(const MetricsReport& report);

/**
 * @returns The line of the journal that says what each system costs a tick on
 * average, all games added up: its total over the ticks of @p report, 0 when
 * the report covers no tick.
 */
[[nodiscard]] std::string systemsLineOf(const MetricsReport& report);

}  // namespace rtype::server
