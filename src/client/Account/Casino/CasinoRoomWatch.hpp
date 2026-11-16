#pragma once

#include <cstdint>
#include <optional>
#include "CasinoRoomReading.hpp"
#include "CasinoRoomState.hpp"
#include "ICasinoLink.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Reads the state of one room of the casino through an ICasinoLink,
 * again CASINO_ROOM_POLL_INTERVAL after each answer, so one read is in flight
 * at most, and times it on the clock of the master.
 *
 * Over HTTP the answer read last is not always the newest: an answer is kept
 * only when it is of the room followed and of a later round, or of the same
 * round at a later serverTime. The clock of the master is that serverTime,
 * moved by the frames counted since the answer was kept.
 */
class CasinoRoomWatch {
 public:
  /** @param link The calls to the master; it must outlive the watch. */
  explicit CasinoRoomWatch(ICasinoLink& link);

  /** @brief Follows @p room, read at once; following the room followed
   * changes nothing, another drops the state kept. */
  void watch(std::uint8_t room);

  /** @brief Follows no room, and drops the state kept. */
  void stop();

  /** @brief Keeps @p reading when it is newer, counts @p elapsed on the clock
   * of the master, and asks the room again once it is due. Called once per
   * frame. */
  void follow(const std::optional<CasinoRoomReading>& reading,
              engine::Duration elapsed);

  /** @returns The state kept, its serverTime the clock of the master now and
   * its phase the one its dates give then (ICasinoChannel::room()). */
  [[nodiscard]] std::optional<network::CasinoRoomState> shown() const;

  /** @returns The room followed. */
  [[nodiscard]] std::optional<std::uint8_t> room() const;

 private:
  /** @brief Keeps the state of @p reading when it answers the newest read
   * and is newer than the state kept. */
  void keep(const std::optional<CasinoRoomReading>& reading);

  ICasinoLink* link_;
  std::optional<std::uint8_t> room_;
  std::uint64_t ask_{};
  bool answered_{true};
  engine::Duration sinceAnswer_{};
  std::optional<network::CasinoRoomState> state_;
  engine::Duration sinceState_{};
};

}  // namespace rtype::client
