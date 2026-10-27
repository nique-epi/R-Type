#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "FinalPilot.hpp"
#include "FlightLogSchedule.hpp"

/** @returns Pilots ranked best first, one per score, numbered from 1, all
 * with a nickname of 16 characters, the longest the protocol allows. */
[[nodiscard]] std::vector<rtype::client::FinalPilot> pilotsScoring(
    const std::vector<std::uint32_t>& scores);

/** @returns The moments of a flight log of @p pilotCount pilots and the taunt
 * of the design, MISSION FAILED / SUCCESSFULLY, 26 letters. */
[[nodiscard]] rtype::client::FlightLogSchedule designScheduleOf(
    std::size_t pilotCount);
