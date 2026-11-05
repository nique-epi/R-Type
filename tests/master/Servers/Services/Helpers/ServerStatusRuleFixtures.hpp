#pragma once

#include <optional>
#include "ServerMeasures.hpp"
#include "StatusEvidence.hpp"

/** @returns Measures with @p tickP95 and @p loss, every other one not taken. */
[[nodiscard]] rtype::network::ServerMeasures measuresWith(
    std::optional<double> tickP95, std::optional<double> loss);

/** @returns What the master knows of a server heard at HEARD_AT with
 * @p measures, not in maintenance. */
[[nodiscard]] rtype::master::StatusEvidence heardWith(
    const rtype::network::ServerMeasures& measures);
