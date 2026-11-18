#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoBetsDesk.hpp"
#include "CasinoReadings.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "CasinoRoomWatch.hpp"
#include "CasinoSeatKeeper.hpp"
#include "IAccountSession.hpp"
#include "ICasinoChannel.hpp"
#include "ICasinoLink.hpp"
#include "IWallet.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The casino of the master for the player of an IAccountSession,
 * played through an ICasinoLink: the room entered is read every
 * CASINO_ROOM_POLL_INTERVAL after the previous answer (CasinoRoomWatch), the
 * seat in it renewed every network::CASINO_SEAT_RENEWAL_INTERVAL
 * (CasinoSeatKeeper), and the bets laid and read back with what they paid
 * (CasinoBetsDesk), the wallet asked again when the master moved chips.
 *
 * Nothing waits on the master: update(), once per frame, reads the answers
 * of the link and asks what is due.
 */
class CasinoChannel final : public ICasinoChannel {
 public:
  /** @param link The calls to the master, @p session the player and
   * @p wallet its chips; all must outlive the channel. */
  CasinoChannel(ICasinoLink& link, const IAccountSession& session,
                IWallet& wallet);

  [[nodiscard]] std::optional<network::CasinoRoomListing> rooms()
      const override;
  void askRooms() override;
  void enter(std::uint8_t room) override;
  void leave() override;
  [[nodiscard]] std::optional<network::CasinoRoomState> room() const override;
  [[nodiscard]] std::optional<CasinoRefusal> seatRefusal() const override;
  void placeBets(std::uint64_t round,
                 const std::vector<network::RouletteBet>& bets) override;
  [[nodiscard]] const std::optional<BetsReading>& lastBets() const override;
  [[nodiscard]] std::optional<network::RoundBets> ownBets(
      std::uint64_t round) const override;

  /** @brief Reads the answers of the link, copied only when it published
   * one since, counts @p elapsed on the clocks of the room and the seat, and
   * asks what is due. */
  void update(engine::Duration elapsed);

 private:
  ICasinoLink* link_;
  /** @brief The answers of the link as update() copied them last, and the
   * revision of the link they were copied at. */
  CasinoReadings readings_;
  std::uint64_t readRevision_{};
  CasinoRoomWatch watch_;
  CasinoSeatKeeper seat_;
  CasinoBetsDesk desk_;
};

}  // namespace rtype::client
