#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "LobbyState.hpp"
#include "PilotStatus.hpp"
#include "PlayerLeft.hpp"
#include "WorldState.hpp"
#include "WorldStateFilter.hpp"

namespace rtype::client {

/**
 * @brief The pilots of one game, with their score, their lives and their
 * state, as the server tells them: what the status band shows.
 *
 * It starts from the waiting room the game started from: one pilot per player
 * listed there, in the order of their numbers, with its nickname, a score of 0
 * and no lives known yet. A WorldState then sets the score, the lives and the
 * state of every player it lists, a player the waiting room did not list
 * included, seated without a nickname; one older than the newest applied is
 * ignored (see WorldStateFilter), so a late datagram never brings back a life
 * already lost. A PlayerLeft marks its player as gone for good: no later state
 * brings it back. A number outside FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS is
 * ignored wherever it comes from.
 *
 * revision() changes each time a pilot does, so a reader repaints only then.
 * One board serves one game: ticks start again with each game.
 */
class ScoreBoard {
 public:
  /**
   * @param waitingRoom The players of the waiting room the game started from.
   * @param localPlayerNumber The number the server gave this client.
   */
  ScoreBoard(std::span<const network::LobbyPlayer> waitingRoom,
             std::uint8_t localPlayerNumber);

  void apply(const network::WorldState& state);
  void apply(const network::PlayerLeft& departure);

  /** @returns Every pilot, in the order of their numbers. */
  [[nodiscard]] std::span<const PilotStatus> pilots() const;

  /** @returns A count of the changes of the pilots, seating included: equal
   * to the one a reader painted, nothing changed since. */
  [[nodiscard]] std::uint32_t revision() const;

 private:
  /** @returns The pilot of a number, seated with nothing known yet when the
   * board had none; nullptr when the number is no player number. */
  PilotStatus* seatedOrNewPilotOf(std::uint8_t playerNumber);

  /** @brief Seats a pilot with nothing known yet at the place of its number,
   * counting a revision.
   * @returns The pilot seated. */
  PilotStatus& seat(std::uint8_t playerNumber);

  /** @brief Replaces a pilot with what it becomes, counting a revision when
   * that differs. */
  void change(PilotStatus& pilot, const PilotStatus& becomes);

  std::uint8_t localPlayerNumber_{};
  std::vector<PilotStatus> pilots_;
  WorldStateFilter worldStateFilter_;
  std::uint32_t revision_{};
};

}  // namespace rtype::client
