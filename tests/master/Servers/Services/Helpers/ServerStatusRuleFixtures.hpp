#pragma once

#include <optional>
#include "ServerMeasures.hpp"
#include "StatusEvidence.hpp"

/** @returns Measures with @p tickP95 and @p loss, every other one not taken. */
[[nodiscard]] rtype::network::ServerMeasures measuresWith(
    std::optional<double> tickP95, std::optional<double> loss);

/** @returns What the master knows of a server registered at HEARD_AT that
 * beat then with @p measures, answering its probe, not in maintenance. */
[[nodiscard]] rtype::master::StatusEvidence heardWith(
    const rtype::network::ServerMeasures& measures);
