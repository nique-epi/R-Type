#pragma once

#include <cstdint>
#include <optional>
#include "CasinoRefusal.hpp"
#include "IAccountSession.hpp"
#include "ICasinoLink.hpp"
#include "SeatReading.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Keeps the player of an IAccountSession seated in one room of the
 * casino: the seat is asked at once, then again every
 * network::CASINO_SEAT_RENEWAL_INTERVAL, which renews it before the master
 * frees it, or takes it once a full room has a seat free. Signed out, nothing
 * is asked.
 *
 * A newer ask for a seat replaces one the link has not served: a player who
 * enters another room before its seat was left keeps the old seat until the
 * master frees it, network::CASINO_SEAT_LIFETIME after its last renewal.
 */
class CasinoSeatKeeper {
 public:
  /** @param link The calls to the master, and @p session the player; both
   * must outlive the keeper. */
  CasinoSeatKeeper(ICasinoLink& link, const IAccountSession& session);

  /** @brief Seats the player in @p room, and keeps it there. */
  void take(std::uint8_t room);

  /** @brief Gets the player up from the room it sits in, if any. */
  void leave();

  /** @brief Reads @p reading, and renews the seat once @p elapsed brings it
   * due. Called once per frame. */
  void follow(const std::optional<SeatReading>& reading,
              engine::Duration elapsed);

  /** @returns Why the master refused the newest seat asked; nothing once
   * seated, or while it is asked. */
  [[nodiscard]] std::optional<CasinoRefusal> refusal() const;

 private:
  /** @brief Asks the master to put the player where @p seated says in the
   * room kept. */
  void ask(bool seated);

  ICasinoLink* link_;
  const IAccountSession* session_;
  std::optional<std::uint8_t> room_;
  std::uint64_t ask_{};
  engine::Duration sinceAsked_{};
  std::optional<CasinoRefusal> refusal_;
};

}  // namespace rtype::client
