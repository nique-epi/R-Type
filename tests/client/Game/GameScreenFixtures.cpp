#include "GameScreenFixtures.hpp"
#include <cstdint>
#include <string>
#include "ConnectionPhase.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "GameEnded.hpp"
#include "GameScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingWaitingRoom.hpp"
#include "WorldState.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::testing::listed;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::SHIP_POSITION;
using rtype::client::testing::SHIP_TYPE_NUMBER;
using rtype::client::testing::stateListingPlayers;
using rtype::network::GameEnded;
using rtype::network::LobbyPlayer;
using rtype::network::PlayerState;
using rtype::network::WorldState;

void seatThreePilots(RecordingServerConnection& connection,
                     RecordingWaitingRoom& waitingRoom) {
  connection.setStatus(
      {.phase = ConnectionPhase::Accepted, .playerNumber = LOCAL_NUMBER});
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

WorldState stateWithTheShipOfTheFirstPilot(std::uint32_t tick) {
  WorldState state = stateScoringTheFirstPilot(tick);
  state.entities.push_back(listed(SHIP_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  return state;
}

GameEnded endOfThreePilots(rtype::network::WireGameOutcome outcome) {
  return GameEnded{
      .outcome = outcome,
      .players = {{.playerNumber = FIRST_PILOT, .score = FIRST_PILOT_FINAL},
                  {.playerNumber = SECOND_PILOT, .score = SECOND_PILOT_FINAL},
                  {.playerNumber = THIRD_PILOT, .score = THIRD_PILOT_FINAL}}};
}
