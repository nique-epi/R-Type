#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rtype::server {

/**
 * @brief Keeps, for one player, the sequence of the newest datagram accepted
 * and how many of the datagrams the player sent never arrived.
 *
 * A datagram is accepted when its sequence is newer than the newest accepted
 * (see network::isNewerSequence); a duplicate or an older one is refused, so a
 * late copy never undoes a newer one. Each accepted datagram stands for the
 * sequences it skipped over: those are counted as lost, so a datagram that
 * arrives late and is refused stays counted as lost.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class ArrivalLedger {
 public:
  /**
   * @brief Accepts or refuses the datagram of sequence @p sequence.
   * @returns Whether it is newer than the newest accepted so far; the first
   * one is always newer. A refused datagram changes nothing.
   */
  [[nodiscard]] bool accept(std::uint16_t sequence);

  /**
   * @returns The share of datagrams lost over the last complete window of
   * LOSS_WINDOW_SIZE datagrams, from 0 (none lost) to 1 (all lost but the
   * ones that closed the window); nothing until the first window is complete.
   */
  [[nodiscard]] std::optional<double> lossRate() const;

 private:
  void closeWindowWhenFull();

  std::optional<std::uint16_t> newest_;
  std::size_t expected_{0};
  std::size_t received_{0};
  std::optional<double> lossRate_;
};

}  // namespace rtype::server
