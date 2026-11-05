#pragma once

#include <string_view>
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Crosses the playfield towards the ships at its full speed, weaving
 * down and up around the row the body appeared on, and fires as often as its
 * Gun allows.
 *
 * At the end of each tick the body is WAVE_AMPLITUDE * sin(2 * pi * age /
 * WAVE_PERIOD_SECONDS) below its row: the height is computed from the age,
 * never added up tick by tick, so the path does not depend on how the
 * simulation time is cut into steps. The vertical share it writes is the one
 * that lands there, so a weave needs a top speed of at least
 * 2 * pi * WAVE_AMPLITUDE / WAVE_PERIOD_SECONDS (151 logical units per second);
 * a slower body falls behind its weave.
 */
class WeaveBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
