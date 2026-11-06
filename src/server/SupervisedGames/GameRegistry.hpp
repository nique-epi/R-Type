#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "ExitedGame.hpp"
#include "GameOpening.hpp"
#include "GamePlace.hpp"
#include "GameStatus.hpp"
#include "SeatRequest.hpp"
#include "SupervisedGame.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The games the supervisor runs, in the order they were started, each
 * as the supervisor knows it. It holds what the game processes reported, and
 * the handles of the processes; it sends nothing and starts nothing.
 */
class GameRegistry {
 public:
  /**
   * @brief Adds the game of @p opening after the others, Starting, in the
   * lowest place no game of the registry holds, from
   * network::FIRST_HOSTED_GAME_NUMBER to MAXIMUM_RUNNING_GAMES.
   * @throws NoFreeGameSlotException when every place holds a game; nothing is
   * added then. placeForNewPlayer() never asks for a game in that case.
   */
  void open(GameOpening opening);

  /** @returns The game that holds @p linkToken, nullptr when none does. */
  [[nodiscard]] SupervisedGame* findByToken(std::uint64_t linkToken);

  /**
   * @brief Takes @p status as the state, the nicknames and the measures of
   * @p game when @p sequence is newer
   * than the newest status taken so far (16-bit serial number arithmetic),
   * and ignores it otherwise: a status that arrives late must not undo a newer
   * one. A status taken ends the seats of the players it now names (see
   * SeatReservations::settle()); one ignored ends none.
   * @returns Whether it was taken.
   */
  bool record(SupervisedGame& game, std::uint16_t sequence,
              const network::GameStatus& status);

  /**
   * @returns Where the player of @p request goes: the game that holds a seat
   * for its sender while that game still waits for players; else the first
   * game that waits for players and has a free seat, its players and its
   * seats held counted, a game still Starting counting as one that waits
   * (Wait, or Redirect once it reported); else Open while fewer than
   * MAXIMUM_RUNNING_GAMES run, else Full. On Wait and Redirect, the sender
   * holds a seat of that game (see SeatReservations), so the players sent in
   * one burst never get more seats than the game has; a sender that asks
   * again takes no second seat.
   */
  [[nodiscard]] GamePlace placeForNewPlayer(const SeatRequest& request);

  /**
   * @brief Removes the games whose process ended.
   * @returns Each of them, in the order they were started.
   */
  [[nodiscard]] std::vector<ExitedGame> collectExits();

  /** @brief Lets @p elapsed pass: ends the seats held SEAT_RESERVATION_TIMEOUT
   * (see SeatReservations::age()), and kills the games still Starting that
   * waited GAME_START_TIMEOUT: they leave the registry at the next
   * collectExits(). */
  void terminateStale(engine::Duration elapsed);

  /** @brief Kills every game. */
  void terminateAll();

  [[nodiscard]] std::span<const SupervisedGame> all() const;

  /** @returns A number that changes whenever what the master and the
   * information requests are told may have changed: a game opened, a game
   * that left, a status taken. The seats held and the time a game waited do
   * not change it. */
  [[nodiscard]] std::uint64_t revision() const;

 private:
  std::vector<SupervisedGame> games_;
  std::uint64_t revision_{0};
};

}  // namespace rtype::server
