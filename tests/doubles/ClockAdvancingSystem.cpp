#include "ClockAdvancingSystem.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

ClockAdvancingSystem::ClockAdvancingSystem(SimulatedClock& clock,
                                           rtype::engine::Duration step)
    : clock_(&clock), step_(step) {}

void ClockAdvancingSystem::update(
    [[maybe_unused]] rtype::engine::ComponentRegistry& components,
    [[maybe_unused]] rtype::engine::Duration elapsed) {
  clock_->advance(step_);
}
