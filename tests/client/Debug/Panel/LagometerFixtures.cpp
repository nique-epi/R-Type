#include "LagometerFixtures.hpp"
#include <cstdint>
#include <optional>
#include "ClockRecalibrations.hpp"
#include "LagometerReading.hpp"
#include "LagometerTestConstants.hpp"
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
                          .pastTheNewest = 0,
                          .delayTicks = std::nullopt,
                          .recalibrations = ClockRecalibrations{}};
}

LagometerReading sampleReading(RoundTripSample sample,
                               SessionArrivals arrivals) {
  LagometerReading reading = frameReading(engine::Duration::zero());
  reading.newestRoundTrip = sample;
  reading.arrivals = arrivals;
  return reading;
}

LagometerReading trafficReading(engine::Duration frame,
                                std::uint64_t bytesReceived) {
  LagometerReading reading = frameReading(frame);
  reading.bytesReceived = bytesReceived;
  return reading;
}

LagometerReading interpolationReading(engine::Duration frame,
                                      std::uint64_t pastTheNewest) {
  LagometerReading reading = frameReading(frame);
  reading.pastTheNewest = pastTheNewest;
  reading.delayTicks = SIX_TICKS;
  return reading;
}

LagometerReading delayedReading(engine::Duration frame, double delayTicks) {
  LagometerReading reading = frameReading(frame);
  reading.delayTicks = delayTicks;
  return reading;
}

}  // namespace rtype::client::testing
