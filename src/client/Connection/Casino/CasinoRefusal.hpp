#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief Why the master took no seat or no bets: BetsClosed when the bets of
 * the round closed, NotSeated when the player sits in no seat of the room,
 * RuleBroken for a bet off the cloth or past the caps of a round,
 * NotEnoughChips when the bets hold more chips than the wallet, RoomFull when
 * every seat is taken, SignedOut when the session of the token ended,
 * MasterRefused for any other refusal or an answer the client cannot read,
 * MasterUnreachable when no master answered.
 */
enum class CasinoRefusal : std::uint8_t {
  BetsClosed,
  NotSeated,
  RuleBroken,
  NotEnoughChips,
  RoomFull,
  SignedOut,
  MasterRefused,
  MasterUnreachable,
};

}  // namespace rtype::client
