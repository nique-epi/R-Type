#include "GameScreenFixtures.hpp"
#include <cstdint>
#include "ConnectionStatus.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "GameScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "RecordingServerConnection.hpp"
#include "WorldState.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::stateListingPlayers;
using rtype::network::LobbyPlayer;
using rtype::network::PlayerState;
using rtype::network::WorldState;

void seatThreePilots(RecordingServerConnection& connection) {
  connection.setStatus({.phase = ConnectionStatus::Phase::Accepted,
                        .playerNumber = LOCAL_NUMBER});
  connection.setLobbyPlayers(
      {LobbyPlayer{
           .playerNumber = FIRST_PILOT, .ready = false, .nickname = "ACE"},
       LobbyPlayer{
           .playerNumber = SECOND_PILOT, .ready = false, .nickname = "NOVA"},
       LobbyPlayer{
           .playerNumber = THIRD_PILOT, .ready = false, .nickname = "KAI"}});
}

WorldState stateScoringTheFirstPilot(std::uint32_t tick) {
  return stateListingPlayers(tick, {PlayerState{.playerNumber = FIRST_PILOT,
                                                .shipEntityId = SHIP_ID,
                                                .lives = PILOT_LIVES,
                                                .score = PILOT_SCORE,
                                                .invulnerable = false}});
}
