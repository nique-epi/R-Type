#pragma once

#include <cstdint>
#include <optional>

namespace rtype::client {

/**
 * @brief Counts how the game showed the other entities, frame after frame:
 * how many frames drew a tick past the newest state received, moving the
 * entities on from their last two states or holding them there once
 * MAXIMUM_EXTRAPOLATION ran out, and how far behind the server the newest
 * frame drew. It outlives the games, so its
 * totals cover the whole run; each game starts it over on its own ticks.
 *
 * Not thread-safe: use it from the frame loop.
 */
class InterpolationTally {
 public:
  /** @brief A new game starts: its ticks start again, and nothing is drawn
   * yet. The totals are kept. */
  void startGame();

  /** @brief Takes the tick of a state received; a tick not newer than the
   * newest one changes nothing. */
  void observe(std::uint32_t tick);

  /**
   * @brief Counts a frame that drew @p renderTick while the server was at
   * @p serverTick. A frame before the first state, or that drew nothing,
   * counts nothing.
   */
  void countFrame(std::optional<double> renderTick,
                  std::optional<double> serverTick);

  /** @returns The frames that drew a tick past the newest state since the
   * client started. */
  [[nodiscard]] std::uint64_t pastTheNewest() const;

  /** @returns How many ticks behind the server the newest frame drew;
   * nothing before a frame of the current game drew with the tick of the
   * server known. */
  [[nodiscard]] std::optional<double> delayTicks() const;

 private:
  std::uint64_t pastTheNewest_{0};
  std::optional<std::uint32_t> newestTick_;
  std::optional<double> delayTicks_;
};

}  // namespace rtype::client
