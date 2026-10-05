#pragma once

#include "IClock.hpp"

/**
 * @brief Clock that only moves when the test advances it.
 */
class SimulatedClock final : public rtype::engine::IClock {
 public:
  rtype::engine::Duration now() const override { return current_; }
  void advance(rtype::engine::Duration elapsed) { current_ += elapsed; }

 private:
  rtype::engine::Duration current_{};
};
