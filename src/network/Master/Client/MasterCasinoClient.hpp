#pragma once

#include <cstdint>
#include <string>
#include "BetsReceipt.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "MasterTransport.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"
#include "RoundBets.hpp"

namespace rtype::network {

/**
 * @brief What the game client calls to play the roulette rooms of the master:
 * one function per route of the casino. The rooms and their state are open to
 * anyone; the seats and the bets take the token of a session, sent as Bearer
 * and never kept, and are answered over HTTPS only.
 *
 * Every call waits for the answer, as MasterTransport does, so it is made
 * away from the frame loop, and throws as MasterTransport::send() does;
 * InvalidMasterMessageException for an answer that is not the message of its
 * route. One call at a time.
 */
class MasterCasinoClient {
 public:
  /** @throws InvalidMasterUrlException as MasterTransport does. */
  explicit MasterCasinoClient(const std::string& url);

  /** @returns Every room, `GET /api/casino/rooms`. */
  [[nodiscard]] CasinoRoomListing listRooms();

  /** @returns The state of @p room, `GET /api/casino/rooms/{room}`.
   * @throws MasterRefusedException with status 404 for a room the casino
   * does not run. */
  [[nodiscard]] CasinoRoomState readRoom(std::uint8_t room);

  /**
   * @returns The state of @p room once the account of @p token sits in it,
   * `PUT /api/casino/rooms/{room}/seats/me`: taken, or renewed when held.
   * @throws MasterRefusedException with status 409 when every seat is taken,
   * 404 for no such room, 401 for a token the master does not know.
   */
  CasinoRoomState takeSeat(const std::string& token, std::uint8_t room);

  /** @brief Gets the account of @p token up from @p room,
   * `DELETE /api/casino/rooms/{room}/seats/me`; leaving again succeeds. */
  void leaveSeat(const std::string& token, std::uint8_t room);

  /**
   * @returns The bets on the cloth and the chips left once @p bets replace
   * those the account of @p token held on @p round,
   * `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me`.
   * @throws MasterRefusedException with status 409 when the bets of the round
   * are closed or the account sits in no seat (BETS_CLOSED_DETAIL,
   * NOT_SEATED_DETAIL), 422 for bets the cloth, the caps or the wallet refuse
   * (NOT_ENOUGH_CHIPS_DETAIL for the wallet).
   */
  BetsReceipt placeBets(const std::string& token, RoomRound round,
                        const RouletteBets& bets);

  /** @returns The bets the account of @p token holds on @p round, and what
   * they paid once drawn,
   * `GET /api/casino/rooms/{room}/rounds/{round}/bets/me`. */
  [[nodiscard]] RoundBets readOwnBets(const std::string& token,
                                      RoomRound round);

 private:
  MasterTransport transport_;
};

}  // namespace rtype::network
