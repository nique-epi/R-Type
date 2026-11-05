#pragma once

#include "ServerMeasures.hpp"

/** @returns The quiet measures of "Paris 1", but for a tick p95 of
 * @p tickP95, and every measure it does not name not taken. */
[[nodiscard]] rtype::network::ServerMeasures parisMeasuresWith(double tickP95);
