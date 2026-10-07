#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>
#include "IPixelSurface.hpp"
#include "ISkyEvent.hpp"
#include "SkyEventConstants.hpp"
#include "SkyEventKind.hpp"

namespace rtype::client {

/**
 * @brief Starts sky events at random intervals, picks their kind by weight,
 * and shows at most MAXIMUM_SKY_EVENTS of them at once.
 */
class SkyEventScheduler {
 public:
  /** @param seed Seed of every random draw of the events. */
  explicit SkyEventScheduler(std::uint32_t seed);

  /**
   * @brief Starts one event when one is due and the sky is not full, then
   * advances every event and drops those that are over.
   *
   * @param seconds Real elapsed time, in seconds.
   */
  void advance(float seconds);

  /** @brief Paints every event, the oldest first. */
  void paint(IPixelSurface& surface) const;

  /** @returns How many events are shown. */
  [[nodiscard]] std::size_t eventCount() const;

 private:
  /**
   * @returns A new event of that kind. A kind outside SkyEventKind, which the
   * weighted draw never gives, makes a shooting star.
   */
  [[nodiscard]] std::unique_ptr<ISkyEvent> createEvent(SkyEventKind kind);

  std::mt19937 random_;
  std::discrete_distribution<std::size_t> kinds_;
  std::vector<std::unique_ptr<ISkyEvent>> events_;
  float secondsUntilNextEvent_ = FIRST_SKY_EVENT_DELAY;
};

}  // namespace rtype::client
