#include "EdgePulse.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include "EdgePulseConstants.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"
#include "WarningBannerConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The share of the pulse at that time, 0 to 1: a cosine from 0
 * at WARNING_START, 1 half a period later; 0 outside the WARNING screen. */
double pulseShareAt(engine::Duration sinceArrival) {
  if (sinceArrival < WARNING_START || sinceArrival >= WARNING_END) {
    return 0.0;
  }
  const double turns =
      static_cast<double>((sinceArrival - WARNING_START).count()) /
      static_cast<double>(EDGE_PULSE_PERIOD.count());
  return (1.0 - std::cos(EDGE_PULSE_FULL_TURN * turns)) * EDGE_PULSE_HALF;
}

/** @returns The share of the flash of the roar, from 1 when it roars to 0
 * ROAR_EDGE_FLASH_TIME later; 0 without a roar. */
double roarShareAt(std::optional<engine::Duration> sinceRoar) {
  if (!sinceRoar.has_value() || *sinceRoar < engine::Duration::zero() ||
      *sinceRoar >= ROAR_EDGE_FLASH_TIME) {
    return 0.0;
  }
  return 1.0 - (static_cast<double>(sinceRoar->count()) /
                static_cast<double>(ROAR_EDGE_FLASH_TIME.count()));
}

sf::Color edgeColorAt(std::uint8_t alpha, int depth) {
  const int remaining = EDGE_PULSE_DEPTH - depth;
  return {HAZARD_RED.r, HAZARD_RED.g, HAZARD_RED.b,
          static_cast<std::uint8_t>((alpha * remaining) / EDGE_PULSE_DEPTH)};
}

}  // namespace

EdgePulseLook edgePulseAt(engine::Duration sinceArrival,
                          std::optional<engine::Duration> sinceRoar,
                          FlashPreference flashes) {
  if (flashes == FlashPreference::Reduced) {
    return {};
  }
  const double pulse = EDGE_PULSE_PEAK_ALPHA * pulseShareAt(sinceArrival);
  const double roar = ROAR_EDGE_FLASH_ALPHA * roarShareAt(sinceRoar);
  return EdgePulseLook{
      .alpha = static_cast<std::uint8_t>(std::lround(std::max(pulse, roar)))};
}

void paintEdgePulse(IInterfaceCanvas& canvas, EdgePulseLook look) {
  if (look.alpha == 0) {
    return;
  }
  const sf::Vector2i size = ALERT_PLAYFIELD_AREA.size;
  for (int depth = 0; depth < EDGE_PULSE_DEPTH; depth += EDGE_PULSE_STEP) {
    const sf::Color color = edgeColorAt(look.alpha, depth);
    if (color.a == 0) {
      continue;
    }
    canvas.fill(sf::IntRect({depth, 0}, {EDGE_PULSE_STEP, size.y}), color);
    canvas.fill(sf::IntRect({size.x - depth - EDGE_PULSE_STEP, 0},
                            {EDGE_PULSE_STEP, size.y}),
                color);
    canvas.fill(sf::IntRect({0, depth}, {size.x, EDGE_PULSE_STEP}), color);
    canvas.fill(sf::IntRect({0, size.y - depth - EDGE_PULSE_STEP},
                            {size.x, EDGE_PULSE_STEP}),
                color);
  }
}

}  // namespace rtype::client
