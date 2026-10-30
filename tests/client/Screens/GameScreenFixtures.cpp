#include "GameScreenFixtures.hpp"
#include <cstdint>
#include <string>
#include "ConnectionStatus.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "GameEnded.hpp"
#include "GameScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingWaitingRoom.hpp"
#include "WorldState.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::stateListingPlayers;
using rtype::network::GameEnded;
using rtype::network::LobbyPlayer;
using rtype::network::PlayerState;
using rtype::network::WorldState;

void seatThreePilots(RecordingServerConnection& connection,
                     RecordingWaitingRoom& waitingRoom) {
  connection.setStatus({.phase = ConnectionStatus::Phase::Accepted,
                        .playerNumber = LOCAL_NUMBER});
  waitingRoom.setLobbyPlayers(
      {LobbyPlayer{.playerNumber = FIRST_PILOT,
                   .ready = false,
                   .nickname = std::string{FIRST_PILOT_NICKNAME}},
       LobbyPlayer{.playerNumber = SECOND_PILOT,
                   .ready = false,
                   .nickname = std::string{SECOND_PILOT_NICKNAME}},
       LobbyPlayer{.playerNumber = THIRD_PILOT,
                   .ready = false,
                   .nickname = std::string{THIRD_PILOT_NICKNAME}}});
}

WorldState stateScoringTheFirstPilot(std::uint32_t tick) {
  return stateListingPlayers(tick, {PlayerState{.playerNumber = FIRST_PILOT,
                                                .shipEntityId = SHIP_ID,
                                                .lives = PILOT_LIVES,
                                                .score = PILOT_SCORE,
                                                .invulnerable = false}});
}

GameEnded endOfThreePilots() {
  return GameEnded{
      .players = {{.playerNumber = FIRST_PILOT, .score = FIRST_PILOT_FINAL},
                  {.playerNumber = SECOND_PILOT, .score = SECOND_PILOT_FINAL},
                  {.playerNumber = THIRD_PILOT, .score = THIRD_PILOT_FINAL}}};
}
