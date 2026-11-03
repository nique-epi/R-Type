#include "ScoreBoard.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include "LobbyState.hpp"
#include "PilotStatus.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @returns Whether a number names a player of a game. */
bool isPlayerNumber(std::uint8_t number) {
  return number >= network::FIRST_PLAYER_NUMBER &&
         static_cast<std::size_t>(number) <= network::MAXIMUM_PLAYERS;
}

/** @returns Whether a pilot has left the game for good, so no world state
 * changes its state any more. */
bool hasLeft(const PilotStatus& pilot) {
  return pilot.state == PilotState::Left ||
         pilot.state == PilotState::SignalLost;
}

/** @returns The state a world state gives a pilot still in the game. */
PilotState stateListedIn(const network::PlayerState& listed) {
  if (listed.lives == 0) {
    return PilotState::OutOfLives;
  }
  return listed.invulnerable ? PilotState::Respawning : PilotState::Flying;
}

/** @returns A pilot as a world state lists it: its score and lives, and its
 * state unless it has left. */
PilotStatus listedVersionOf(PilotStatus pilot,
                            const network::PlayerState& listed) {
  pilot.score = listed.score;
  pilot.lives = listed.lives;
  if (!hasLeft(pilot)) {
    pilot.state = stateListedIn(listed);
  }
  return pilot;
}

/** @returns The state a departure leaves its pilot in. */
PilotState stateAfter(network::DepartureReason reason) {
  switch (reason) {
    case network::DepartureReason::TimedOut:
      return PilotState::SignalLost;
    case network::DepartureReason::Left:
      break;
  }
  return PilotState::Left;
}

/** @returns A pilot as a departure leaves it: its state after the departure,
 * the rest kept. */
PilotStatus departedVersionOf(PilotStatus pilot,
                              network::DepartureReason reason) {
  pilot.state = stateAfter(reason);
  return pilot;
}

/** @returns A pilot of whom nothing is known yet: no nickname, a score of 0,
 * no lives, flying. */
PilotStatus pilotWithNothingKnown(std::uint8_t playerNumber,
                                  bool isLocalPlayer) {
  return PilotStatus{.playerNumber = playerNumber,
                     .nickname = {},
                     .score = 0,
                     .lives = std::nullopt,
                     .state = PilotState::Flying,
                     .isLocalPlayer = isLocalPlayer};
}

}  // namespace

ScoreBoard::ScoreBoard(std::span<const network::LobbyPlayer> waitingRoom,
                       std::uint8_t localPlayerNumber)
    : localPlayerNumber_(localPlayerNumber) {
  for (const network::LobbyPlayer& player : waitingRoom) {
    PilotStatus* pilot = seatedOrNewPilotOf(player.playerNumber);
    if (pilot != nullptr) {
      pilot->nickname = player.nickname;
    }
  }
}

void ScoreBoard::apply(const network::WorldState& state) {
  if (!worldStateFilter_.accept(state.tick)) {
    return;
  }
  for (const network::PlayerState& listed : state.players) {
    PilotStatus* pilot = seatedOrNewPilotOf(listed.playerNumber);
    if (pilot != nullptr) {
      change(*pilot, listedVersionOf(*pilot, listed));
    }
  }
}

void ScoreBoard::apply(const network::PlayerLeft& departure) {
  PilotStatus* pilot = seatedOrNewPilotOf(departure.playerNumber);
  if (pilot != nullptr) {
    change(*pilot, departedVersionOf(*pilot, departure.reason));
  }
}

std::span<const PilotStatus> ScoreBoard::pilots() const { return pilots_; }

std::uint32_t ScoreBoard::revision() const { return revision_; }

PilotStatus* ScoreBoard::seatedOrNewPilotOf(std::uint8_t playerNumber) {
  if (!isPlayerNumber(playerNumber)) {
    return nullptr;
  }
  const auto found =
      std::ranges::find(pilots_, playerNumber, &PilotStatus::playerNumber);
  return found != pilots_.end() ? &*found : &seat(playerNumber);
}

PilotStatus& ScoreBoard::seat(std::uint8_t playerNumber) {
  const auto place = std::ranges::upper_bound(pilots_, playerNumber, {},
                                              &PilotStatus::playerNumber);
  ++revision_;
  return *pilots_.insert(
      place,
      pilotWithNothingKnown(playerNumber, playerNumber == localPlayerNumber_));
}

void ScoreBoard::change(PilotStatus& pilot, const PilotStatus& becomes) {
  if (pilot != becomes) {
    pilot = becomes;
    ++revision_;
  }
}

}  // namespace rtype::client
