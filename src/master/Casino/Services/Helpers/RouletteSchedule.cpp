#include "RouletteSchedule.hpp"
#include <chrono>
#include <cstdint>
#include "MillisecondDate.hpp"
#include "RouletteDurations.hpp"
#include "RoulettePhase.hpp"
#include "ScheduledRound.hpp"

namespace rtype::master {

namespace {

network::RoulettePhase phaseAt(MillisecondDate now,
                               const ScheduledRound& round) {
  if (now < round.closesAt) {
    return network::RoulettePhase::Betting;
  }
  if (now < round.resultAt) {
    return network::RoulettePhase::Spinning;
  }
  return network::RoulettePhase::Result;
}

}  // namespace

ScheduledRound scheduledRoundAt(MillisecondDate now,
                                const RouletteDurations& durations) {
  const std::chrono::milliseconds cycle =
      durations.betting + durations.spinning + durations.result;
  const auto round = static_cast<std::uint64_t>(now.time_since_epoch() / cycle);
  const MillisecondDate start{cycle * static_cast<std::int64_t>(round)};
  ScheduledRound scheduled{
      .round = round,
      .phase = network::RoulettePhase::Betting,
      .closesAt = start + durations.betting,
      .resultAt = start + durations.betting + durations.spinning,
      .endsAt = start + cycle};
  scheduled.phase = phaseAt(now, scheduled);
  return scheduled;
}

std::uint64_t firstOpenRound(const ScheduledRound& scheduled) {
  if (scheduled.phase == network::RoulettePhase::Betting) {
    return scheduled.round;
  }
  return scheduled.round + 1;
}

MillisecondDate currentMillisecond() {
  return std::chrono::floor<std::chrono::milliseconds>(
      std::chrono::system_clock::now());
}

}  // namespace rtype::master
