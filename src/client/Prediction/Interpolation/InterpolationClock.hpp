#pragma once

#include <cstdint>
#include <optional>
#include "InterpolationConstants.hpp"
#include "RecentValues.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Says which server tick the client shows: the one
 * INTERPOLATION_INTERVALS state intervals before the newest state received, so
 * that two states always surround it.
 *
 * It advances with the real time of the frames and is nudged at each state
 * (CORRECTION_GAIN), so network jitter does not make entities speed up and slow
 * down. The interval is the smallest of the last INTERVAL_WINDOW gaps between
 * the ticks of the states received: it follows --state-rate with no setting.
 * One clock serves one game: the ticks start again with each game.
 *
 * Given the tick the server is at (follow()), it measures how late each state
 * arrives, the tick the server was at when it arrived less the tick the state
 * carries, and shows the tick that many ticks plus INTERPOLATION_INTERVALS
 * intervals behind the server: the delay then follows the worst lateness of
 * the last LATENESS_WINDOW states instead of the arrival of the newest one, so
 * a late state does not move the tick shown back and forth. Without a tick of
 * the server, or while the states it measures are not plausible, it shows the
 * tick behind the newest state, as above.
 */
class InterpolationClock {
 public:
  /** @brief Moves the tick shown on by @p elapsed of real time, never beyond
   * the newest tick received plus MAXIMUM_EXTRAPOLATION_TICKS. Nothing before
   * the first state. */
  void advance(engine::Duration elapsed);

  /** @brief Tells the clock the tick the server is at now, as the clock of the
   * server puts it (see IServerClock::tickAt()); nothing while it is not
   * known. Call it each frame, before observe() and advance(). */
  void follow(std::optional<double> serverTick);

  /** @brief Takes the tick of a state received into account; nothing when it
   * is not newer than the newest one (a late copy, or another part of the
   * same state). */
  void observe(std::uint32_t tick);

  /** @returns The tick to show, fractional; nothing before the first state. */
  [[nodiscard]] std::optional<double> renderTick() const;

  /** @returns The whole tick shown, as a PlayerInput carries it:
   * NO_SHOWN_TICK before the first state, and for a tick shown before tick 1.
   */
  [[nodiscard]] std::uint32_t shownTick() const;

 private:
  /** @returns The tick the delay is counted back from: the tick of the server
   * less the worst lateness measured, or the newest tick received when either
   * is missing. */
  [[nodiscard]] double referenceTick() const;

  /** @returns The reference tick minus the delay: where the tick shown would
   * be if every state had arrived on time. */
  [[nodiscard]] double targetTick() const;

  /** @brief Keeps how late the state of @p tick arrived, when the clock of the
   * server is known and the lateness is plausible. */
  void recordLateness(std::uint32_t tick);

  /** @returns The interval between two states, in ticks. */
  [[nodiscard]] double intervalTicks() const;

  /** @returns @p shown with CORRECTION_GAIN of its error to the target taken
   * back, or all of it beyond SNAP_INTERVALS intervals and when the first
   * interval was just measured, which replaces the assumed one. */
  [[nodiscard]] double pulledTowardsTarget(double shown) const;

  std::optional<double> renderTick_;
  std::optional<double> serverTick_;
  std::uint32_t newestTick_{0};
  RecentValues<std::uint32_t, INTERVAL_WINDOW> gaps_;
  RecentValues<double, LATENESS_WINDOW> lateness_;
};

}  // namespace rtype::client
