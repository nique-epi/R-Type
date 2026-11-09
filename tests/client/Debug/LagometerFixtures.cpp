#include "LagometerFixtures.hpp"
#include <cstdint>
#include <optional>
#include "InterpolationCounts.hpp"
#include "LagometerReading.hpp"
#include "RoundTripSample.hpp"
#include "SessionArrivals.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

LagometerReading frameReading(engine::Duration frame) {
  return LagometerReading{.frame = frame,
                          .figures = std::nullopt,
                          .newestRoundTrip = std::nullopt,
                          .arrivals = SessionArrivals{},
                          .bytesReceived = 0,
                          .bytesSent = 0,
                          .interpolation = InterpolationCounts{},
                          .delayTicks = std::nullopt};
}

LagometerReading sampleReading(RoundTripSample sample,
                               SessionArrivals arrivals) {
  return LagometerReading{.frame = engine::Duration::zero(),
                          .figures = std::nullopt,
                          .newestRoundTrip = sample,
                          .arrivals = arrivals,
                          .bytesReceived = 0,
                          .bytesSent = 0,
                          .interpolation = InterpolationCounts{},
                          .delayTicks = std::nullopt};
}

LagometerReading trafficReading(engine::Duration frame,
                                std::uint64_t bytesReceived) {
  return LagometerReading{.frame = frame,
                          .figures = std::nullopt,
                          .newestRoundTrip = std::nullopt,
                          .arrivals = SessionArrivals{},
                          .bytesReceived = bytesReceived,
                          .bytesSent = 0,
                          .interpolation = InterpolationCounts{},
                          .delayTicks = std::nullopt};
}

LagometerReading interpolationReading(engine::Duration frame,
                                      InterpolationCounts interpolation,
                                      double delayTicks) {
  return LagometerReading{.frame = frame,
                          .figures = std::nullopt,
                          .newestRoundTrip = std::nullopt,
                          .arrivals = SessionArrivals{},
                          .bytesReceived = 0,
                          .bytesSent = 0,
                          .interpolation = interpolation,
                          .delayTicks = delayTicks};
}

}  // namespace rtype::client::testing
