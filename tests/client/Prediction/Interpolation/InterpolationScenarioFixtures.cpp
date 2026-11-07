#include "InterpolationScenarioFixtures.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "InterpolationClock.hpp"
#include "InterpolationTestConstants.hpp"
#include "Position.hpp"
#include "PositionHistory.hpp"
#include "PositionSample.hpp"

namespace rtype::client::testing {

namespace {

enum class Showing : std::uint8_t { Interpolated, NewestState };

struct Receiver {
  InterpolationClock clock;
  PositionHistory history;
  std::optional<game::Position> newest;
};

std::uint32_t tickOf(const InterpolationScenario& scenario, std::size_t state) {
  return FIRST_SCENARIO_TICK +
         (static_cast<std::uint32_t>(state) * scenario.ticksBetweenStates);
}

game::Position positionAt(std::uint32_t tick) {
  return game::Position{.x = UNITS_PER_TICK * static_cast<float>(tick),
                        .y = 0.0F};
}

std::uint32_t arrivalTickOf(const InterpolationScenario& scenario,
                            std::size_t state) {
  const bool late = state % JITTER_PERIOD == 0;
  return tickOf(scenario, state) + (late ? LATE_ARRIVAL_TICKS : 0);
}

bool isLost(const InterpolationScenario& scenario, std::size_t state) {
  return std::ranges::find(scenario.lostStates, state) !=
         scenario.lostStates.end();
}

void deliverArrivals(const InterpolationScenario& scenario, std::uint32_t frame,
                     Receiver& receiver) {
  for (std::size_t state = 0; state < STATES_IN_THE_SCENARIO; ++state) {
    if (isLost(scenario, state) || arrivalTickOf(scenario, state) != frame) {
      continue;
    }
    const std::uint32_t tick = tickOf(scenario, state);
    receiver.clock.observe(tick);
    receiver.history.record(
        PositionSample{.tick = tick, .position = positionAt(tick)});
    receiver.newest = positionAt(tick);
  }
}

std::optional<float> shownX(const Receiver& receiver, Showing showing) {
  if (showing == Showing::NewestState) {
    return receiver.newest.has_value()
               ? std::optional<float>{receiver.newest->x}
               : std::nullopt;
  }
  if (!receiver.clock.renderTick().has_value()) {
    return std::nullopt;
  }
  return receiver.history.at(receiver.clock.renderTick().value()).value().x;
}

std::vector<float> framesOf(const InterpolationScenario& scenario,
                            Showing showing) {
  Receiver receiver;
  std::vector<float> shown;
  const std::uint32_t lastFrame =
      arrivalTickOf(scenario, STATES_IN_THE_SCENARIO - 1);
  for (std::uint32_t frame = 0; frame <= lastFrame; ++frame) {
    receiver.clock.advance(ONE_TICK);
    deliverArrivals(scenario, frame, receiver);
    if (const std::optional<float> abscissa = shownX(receiver, showing)) {
      shown.push_back(abscissa.value());
    }
  }
  return shown;
}

float largestStep(const InterpolationScenario& scenario, Showing showing) {
  const std::vector<float> shown = framesOf(scenario, showing);
  float largest = 0.0F;
  for (std::size_t frame = FRAMES_BEFORE_THE_CLOCK_SETTLES;
       frame < shown.size(); ++frame) {
    largest = std::max(largest, std::abs(shown[frame] - shown[frame - 1]));
  }
  return largest;
}

}  // namespace

std::vector<std::size_t> everyTwentiethState() {
  std::vector<std::size_t> lost;
  for (std::size_t state = EVERY_TWENTIETH_STATE - 1;
       state < STATES_IN_THE_SCENARIO; state += EVERY_TWENTIETH_STATE) {
    lost.push_back(state);
  }
  return lost;
}

std::vector<std::size_t> everyTwentiethStateAndTwoInARow() {
  std::vector<std::size_t> lost = everyTwentiethState();
  lost.push_back(FIRST_OF_TWO_LOST_IN_A_ROW);
  lost.push_back(SECOND_OF_TWO_LOST_IN_A_ROW);
  return lost;
}

float largestStepInterpolated(const InterpolationScenario& scenario) {
  return largestStep(scenario, Showing::Interpolated);
}

float largestStepOfTheNewestState(const InterpolationScenario& scenario) {
  return largestStep(scenario, Showing::NewestState);
}

}  // namespace rtype::client::testing
