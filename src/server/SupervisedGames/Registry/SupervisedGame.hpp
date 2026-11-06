#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "GameLifecycle.hpp"
#include "GameStatusMeasures.hpp"
#include "IChildProcess.hpp"
#include "SeatReservations.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief What the supervisor knows of one game process: one line of its
 * registry. */
struct SupervisedGame {
  /** @brief Its number in the journal and in the name of its journal file,
   * never given twice. */
  std::uint32_t number{};
  /** @brief Its place, network::FIRST_HOSTED_GAME_NUMBER to
   * MAXIMUM_RUNNING_GAMES: its number for the master, given again once the
   * game left. */
  std::uint16_t slot{};
  GameLifecycle lifecycle{GameLifecycle::Starting};
  /** @brief The nicknames of its connected players, as its newest status
   * said; none before the first one. The seats it has for a new player are
   * network::MAXIMUM_PLAYERS less these and the reservations. */
  std::vector<std::string> nicknames;
  /** @brief The measures of its newest status; none before the first one. */
  network::GameStatusMeasures measures;
  /** @brief The UDP port the game listens on, chosen by the supervisor. */
  std::uint16_t port{};
  /** @brief The token the game and the supervisor put in their datagrams. */
  std::uint64_t linkToken{};
  /** @brief The sequence of the newest status accepted; nothing before the
   * first one. */
  std::optional<std::uint16_t> newestSequence;
  /** @brief The time since the launch, counted while the game is Starting. */
  engine::Duration sinceLaunch{};
  /** @brief The seats it holds for players the supervisor sent it, which its
   * newest status does not count yet. */
  SeatReservations reservations;
  /** @brief Who asked for a new game and got this one; nothing when it opened
   * for a player who took any game. A new game asked for again by its creator
   * goes to it while nobody else joined it (see GameRegistry::placeFor()). */
  std::optional<network::Endpoint> creator;
  std::unique_ptr<IChildProcess> process;
};

}  // namespace rtype::server
