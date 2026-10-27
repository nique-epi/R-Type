#pragma once

#include "IPixelSurface.hpp"
#include "SkyLight.hpp"

namespace rtype::client {

/**
 * @brief Something that crosses or lights up the sky for a while: a shooting
 * star, a flare, an asteroid or a comet.
 */
class ISkyEvent {
 public:
  virtual ~ISkyEvent() = default;

  /**
   * @param eventSeconds Elapsed time on the sky event clock, which runs
   * SKY_EVENT_CLOCK_RATE times faster than real time.
   */
  virtual void advance(float eventSeconds) = 0;

  /** @returns Whether the event has left the sky or faded out. */
  [[nodiscard]] virtual bool isOver() const = 0;

  /** @brief Paints the event as it stands. In a sky of SkyLight::Steady,
   * nothing of it twinkles nor flickers, whatever advance() drew for the
   * frame. */
  virtual void paint(IPixelSurface& surface, SkyLight light) const = 0;
};

}  // namespace rtype::client
