#pragma once

#include "ISystem.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

/** @brief System double that takes a known time: each update moves the clock of
 * the test on by the step it was given. */
class ClockAdvancingSystem final : public rtype::engine::ISystem {
 public:
  /** @param clock Must outlive the system. */
  ClockAdvancingSystem(SimulatedClock& clock, rtype::engine::Duration step);

  void update(rtype::engine::ComponentRegistry& components,
              rtype::engine::Duration elapsed) override;

 private:
  SimulatedClock* clock_;
  rtype::engine::Duration step_;
};
