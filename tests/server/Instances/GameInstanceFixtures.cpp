#include "GameInstanceFixtures.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ByteWriter.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "DebugJournal.hpp"
#include "Entity.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameInstanceParts.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "RtypeGameModules.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "Spawning/WavePlanFixtures.hpp"
#include "TimerScheduler.hpp"
#include "UnreliableNetwork.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

std::vector<network::OutgoingDatagram> ofType(
    network::MessageType type,
    const std::vector<network::OutgoingDatagram>& datagrams) {
  std::vector<network::OutgoingDatagram> kept;
  for (const network::OutgoingDatagram& datagram : datagrams) {
    if (onlyMessageTypeOf(datagram) == type) {
      kept.push_back(datagram);
    }
  }
  return kept;
}

network::PacketHeader sessionHeader(std::size_t player) {
  network::PacketHeader header;
  header.sessionToken = PLAYER_SESSION_TOKENS.at(player);
  return header;
}

std::size_t bydosAmong(
    const std::vector<std::uint32_t>& removed,
    const std::vector<network::OutgoingDatagram>& datagrams) {
  std::size_t bydos = 0;
  for (const network::OutgoingDatagram& datagram :
       ofType(network::MessageType::EntitySpawned, datagrams)) {
    const auto spawned = onlyMessageIn<network::EntitySpawned>(datagram);
    if (spawned.entityType == network::WireEntityType::Bydo &&
        std::ranges::count(removed, spawned.entityId) == 1) {
      ++bydos;
    }
  }
  return bydos;
}

std::size_t bydosIn(game::World& world) {
  std::size_t bydos = 0;
  world.components().forEach<game::EntityType, game::Position>(
      [&bydos](engine::Entity /*entity*/, const game::EntityType& type,
               const game::Position& /*position*/) {
        if (type == game::EntityType::Enemy) {
          ++bydos;
        }
      });
  return bydos;
}

GameInstanceTest::GameInstanceTest()
    : game_(GameInstanceParts{.send =
                                  [this](network::OutgoingDatagram datagram) {
                                    sent_.push_back(std::move(datagram));
                                  },
                              .settings = GameSettings{.seed = SOME_SEED,
                                                       .waves = testWavePlan()},
                              .events = events_,
                              .timers = timers_},
            rtypeGameModules()) {}

ConnectedPlayer& GameInstanceTest::join(std::size_t index,
                                        std::uint64_t sessionToken) {
  ConnectedPlayer* const player = game().roster().add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, sessionToken);
  game().onPlayerJoined(*player);
  livePlayers_.push_back(
      LivePlayer{.index = index, .sessionToken = sessionToken});
  return *player;
}

void GameInstanceTest::silence(std::size_t index) {
  std::erase_if(livePlayers_, [index](const LivePlayer& player) {
    return player.index == index;
  });
}

void GameInstanceTest::leave(std::size_t index) {
  silence(index);
  queueDisconnectionFrom(index);
}

void GameInstanceTest::queueDisconnectionFrom(std::size_t index) {
  queue(network::IncomingDatagram{.sender = playerEndpoint(index),
                                  .payload = disconnectionDatagram()});
}

engine::TimerScheduler& GameInstanceTest::timers() { return timers_; }

const std::vector<network::OutgoingDatagram>& GameInstanceTest::sentDatagrams()
    const {
  return sent_;
}

std::vector<network::PlayerLeft> GameInstanceTest::playerLeftSentTo(
    const std::vector<network::OutgoingDatagram>& datagrams,
    std::size_t index) {
  std::vector<network::PlayerLeft> received;
  for (const network::OutgoingDatagram& datagram :
       ofType(network::MessageType::PlayerLeft,
              testing::sentTo(datagrams, playerEndpoint(index)))) {
    received.push_back(onlyMessageIn<network::PlayerLeft>(datagram));
  }
  return received;
}

void GameInstanceTest::sendReady(const SentReadyState& sent) {
  network::PacketHeader header;
  header.sessionToken = sent.sessionToken;
  header.sequence = sent.sequence;
  queue(network::IncomingDatagram{
      .sender = playerEndpoint(sent.player),
      .payload = network::DatagramCodec::encode(
          header, network::ReadyState{.ready = sent.ready})});
}

void GameInstanceTest::queue(network::IncomingDatagram datagram) {
  incoming_.push_back(std::move(datagram));
}

void GameInstanceTest::tick() {
  for (const LivePlayer& player : livePlayers_) {
    network::PacketHeader header;
    header.sessionToken = player.sessionToken;
    queue(network::IncomingDatagram{.sender = playerEndpoint(player.index),
                                    .payload = heartbeatDatagram(header)});
  }
  game().onTick(incoming_);
  incoming_.clear();
}

void GameInstanceTest::runTicks(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    tick();
  }
}

void GameInstanceTest::startGameWith(
    std::initializer_list<std::size_t> indexes) {
  for (const std::size_t index : indexes) {
    join(index);
    sendReady({.player = index, .ready = true, .sequence = FIRST_SEQUENCE});
  }
  tick();
  ASSERT_EQ(game().state(), GameState::InProgress);
}

void GameInstanceTest::startGameWithOnePlayer() {
  startGameWith({FIRST_PLAYER});
}

void GameInstanceTest::timeOut(std::size_t index) {
  silence(index);
  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS + 1);
}

std::uint32_t GameInstanceTest::shipOf(std::size_t index) {
  const ConnectedPlayer* const player =
      game().roster().find(playerEndpoint(index));
  return player == nullptr ? identifiers::NO_ENTITY_ID : player->shipEntityId;
}

std::vector<network::OutgoingDatagram> GameInstanceTest::sentTo(
    std::size_t index) const {
  return testing::sentTo(sent_, playerEndpoint(index));
}

GameInstance& GameInstanceTest::game() { return game_; }

engine::EventBus& GameInstanceTest::events() { return events_; }

GameInstanceInputTest::GameInstanceInputTest() {
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    join(player, PLAYER_SESSION_TOKENS.at(player));
  }
}

std::vector<std::byte> GameInstanceInputTest::nextInputDatagram(
    std::size_t player, std::uint8_t actions,
    const network::PacketHeader& header) {
  ++inputsSent_.at(player);
  return inputDatagram(
      network::PlayerInput{.inputNumber = inputsSent_.at(player),
                           .actions = actions},
      header);
}

void GameInstanceInputTest::send(std::size_t player, std::uint8_t actions) {
  send(player, actions, sessionHeader(player));
}

void GameInstanceInputTest::send(std::size_t player, std::uint8_t actions,
                                 const network::PacketHeader& header) {
  sendDatagram(player, nextInputDatagram(player, actions, header));
}

void GameInstanceInputTest::sendDatagram(std::size_t player,
                                         std::vector<std::byte> datagram) {
  network_.send(network::IncomingDatagram{.sender = playerEndpoint(player),
                                          .payload = std::move(datagram)});
}

void GameInstanceInputTest::deliver(
    std::initializer_list<std::size_t> sendIndexes) {
  for (network::IncomingDatagram& arrival : network_.arrivals(sendIndexes)) {
    queue(std::move(arrival));
  }
  tick();
}

void GameInstanceInputTest::deliverInTicks(
    std::span<const network::IncomingDatagram> arrivals) {
  while (!arrivals.empty()) {
    const std::size_t count = std::min(DATAGRAMS_PER_TICK, arrivals.size());
    for (const network::IncomingDatagram& arrival : arrivals.first(count)) {
      queue(arrival);
    }
    tick();
    arrivals = arrivals.subspan(count);
  }
}

std::uint32_t GameInstanceInputTest::lastInputNumber(std::size_t player) const {
  return inputsSent_.at(player);
}

std::optional<network::PlayerInput> GameInstanceInputTest::newestInputOf(
    std::size_t player) {
  const ConnectedPlayer* const connected =
      game().roster().find(playerEndpoint(player));
  if (connected == nullptr) {
    ADD_FAILURE() << "player " << player << " is not in the roster";
    return std::nullopt;
  }
  return connected->newestInput;
}

std::optional<std::uint8_t> GameInstanceInputTest::heldActions(
    std::size_t player) {
  const std::optional<network::PlayerInput> newest = newestInputOf(player);
  if (!newest.has_value()) {
    return std::nullopt;
  }
  return newest->actions;
}

const UnreliableNetwork& GameInstanceInputTest::network() const {
  return network_;
}

void GameInstanceMovementTest::startGame() {
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    sendReady({.player = player,
               .ready = true,
               .sequence = FIRST_SEQUENCE,
               .sessionToken = PLAYER_SESSION_TOKENS.at(player)});
  }
  tick();
  ASSERT_EQ(game().state(), GameState::InProgress);
}

std::uint32_t GameInstanceMovementTest::shipIdOf(std::size_t player) {
  const ConnectedPlayer* const connected =
      game().roster().find(playerEndpoint(player));
  if (connected == nullptr) {
    ADD_FAILURE() << "player " << player << " is not in the roster";
    return identifiers::NO_ENTITY_ID;
  }
  return connected->shipEntityId;
}

game::Position GameInstanceMovementTest::shipPositionOf(std::size_t player) {
  const game::Position* const ship =
      game().world().shipPositionOf(shipIdOf(player));
  if (ship == nullptr) {
    ADD_FAILURE() << "player " << player << " has no ship";
    return game::Position{.x = 0.0F, .y = 0.0F};
  }
  return *ship;
}

void PrintTo(const HeldArrow& arrow, std::ostream* output) {
  *output << arrow.name;
}

void GameInstanceLogTest::queueStrangersInput() {
  queue(network::IncomingDatagram{
      .sender = playerEndpoint(STRANGER),
      .payload = inputDatagram(
          network::PlayerInput{.inputNumber = network::FIRST_INPUT_NUMBER,
                               .actions = HOLDS_UP},
          sessionHeader(FIRST_PLAYER))});
}

std::vector<std::string> GameInstanceLogTest::dropLines() const {
  return journal_.linesHolding(DROPPED_DATAGRAM_LINE_TEXT);
}

const DebugJournal& GameInstanceLogTest::journal() const { return journal_; }

std::vector<network::MessageType> messageTypesOf(
    const std::vector<network::OutgoingDatagram>& datagrams) {
  std::vector<network::MessageType> types;
  types.reserve(datagrams.size());
  for (const network::OutgoingDatagram& datagram : datagrams) {
    types.push_back(onlyMessageTypeOf(datagram));
  }
  return types;
}

std::optional<network::EntityState> entityOf(const network::WorldState& state,
                                             std::uint32_t entityId) {
  for (const network::EntityState& entity : state.entities) {
    if (entity.entityId == entityId) {
      return entity;
    }
  }
  return std::nullopt;
}

network::LobbyState roomOfTheFirstPlayerNotReady() {
  return network::LobbyState{.players = {network::LobbyPlayer{
                                 .playerNumber = network::FIRST_PLAYER_NUMBER,
                                 .ready = false,
                                 .nickname = std::string{PLAYER_NICKNAME}}}};
}

std::vector<std::byte> inputPayload(const network::PlayerInput& input) {
  network::ByteWriter payload;
  payload.writeUint32(input.inputNumber);
  payload.writeUint8(input.actions);
  return payload.bytes();
}

}  // namespace rtype::server::testing
