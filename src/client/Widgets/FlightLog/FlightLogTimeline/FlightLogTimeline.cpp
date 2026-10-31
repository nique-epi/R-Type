#include "FlightLogTimeline.hpp"
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "FinalPilot.hpp"
#include "FlightLogSchedule.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimingConstants.hpp"
#include "GlowingCallPainting.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"

namespace rtype::client {

namespace {

/** @returns @p step, @p count times. */
engine::Duration times(engine::Duration step, std::size_t count) {
  return step * static_cast<engine::Duration::rep>(count);
}

}  // namespace

FlightLogSchedule flightLogScheduleOf(std::span<const FinalPilot> pilots,
                                      std::span<const std::string> taunt) {
  const engine::Duration openStart =
      TAUNT_START + typingTimeOf(TAUNT_LETTER_TIME, letterCountOf(taunt)) +
      TAUNT_HOLD;
  const engine::Duration opened = openStart + OPEN_TIME;
  const engine::Duration rowsStart = opened + ROWS_DELAY;
  const std::size_t laterRows = pilots.empty() ? 0 : pilots.size() - 1;
  const engine::Duration promptStart =
      rowsStart + times(ROW_STEP, laterRows) + ROW_TIME + PROMPT_DELAY;
  return FlightLogSchedule{.openStart = openStart,
                           .opened = opened,
                           .stamped = opened + STAMP_DELAY,
                           .rowsStart = rowsStart,
                           .promptStart = promptStart,
                           .ready = promptStart + callTypingTimeOf(PROMPT_TEXT),
                           .rowCount = pilots.size()};
}

engine::Duration rowStartOf(const FlightLogSchedule& schedule,
                            std::size_t row) {
  return schedule.rowsStart + times(ROW_STEP, row);
}

std::size_t letterCountOf(std::span<const std::string> lines) {
  std::size_t letters = 0;
  for (const std::string& line : lines) {
    letters += letterCountOf(std::string_view{line});
  }
  return letters;
}

std::vector<std::string_view> cuesBetween(const FlightLogSchedule& schedule,
                                          engine::Duration from,
                                          engine::Duration until) {
  const auto crossed = [from, until](engine::Duration moment) {
    return from < moment && moment <= until;
  };
  std::vector<std::string_view> cues;
  if (crossed(LANDING)) {
    cues.emplace_back(HEAVY_IMPACT_SOUND_ID);
  }
  if (crossed(schedule.stamped)) {
    cues.emplace_back(LOGO_IMPACT_SOUND_ID);
  }
  for (std::size_t row = 0; row < schedule.rowCount; ++row) {
    if (crossed(rowStartOf(schedule, row))) {
      cues.emplace_back(CLICK_SOUND_ID);
    }
  }
  return cues;
}

}  // namespace rtype::client
