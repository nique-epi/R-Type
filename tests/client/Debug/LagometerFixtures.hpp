#pragma once

#include <cstdint>
#include "InterpolationCounts.hpp"
#include "LagometerReading.hpp"
#include "RoundTripSample.hpp"
#include "SessionArrivals.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @returns A frame of @p frame that read nothing else. */
[[nodiscard]] LagometerReading frameReading(engine::Duration frame);

/** @returns A frame of no time that read @p sample and @p arrivals. */
[[nodiscard]] LagometerReading sampleReading(RoundTripSample sample,
                                             SessionArrivals arrivals);

/** @returns A frame of @p frame after which the socket had received
 * @p bytesReceived. */
[[nodiscard]] LagometerReading trafficReading(engine::Duration frame,
                                              std::uint64_t bytesReceived);

/** @returns A frame of @p frame after which the game had drawn
 * @p interpolation, its newest frame @p delayTicks behind the server. */
[[nodiscard]] LagometerReading interpolationReading(
    engine::Duration frame, InterpolationCounts interpolation,
    double delayTicks);

}  // namespace rtype::client::testing
