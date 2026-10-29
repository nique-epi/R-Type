#pragma once

#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DebugJournal.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameInstanceTestConstants.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "Position.hpp"
#include "RtypeRulesModule.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimerScheduler.hpp"
#include "UnreliableNetwork.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

/** @brief A ReadyState sent by a player, with the header it travels under. */
struct SentReadyState {
  std::size_t player{};
  bool ready{};
  std::uint16_t sequence{};
  std::uint64_t sessionToken{SESSION_TOKEN};
};

/** @returns Those of @p datagrams whose only message is of type @p type, in
 * order. */
[[nodiscard]] std::vector<network::OutgoingDatagram> ofType(
    network::MessageType type,
    const std::vector<network::OutgoingDatagram>& datagrams);

/** @returns A header carrying the session token of @p player. */
[[nodiscard]] network::PacketHeader sessionHeader(std::size_t player);

/** @returns How many of the network ids in @p removed belong to Bydos, as the
 * EntitySpawned among @p datagrams announced them; the missiles the Bydos fire
 * are not counted. */
[[nodiscard]] std::size_t bydosAmong(
    const std::vector<std::uint32_t>& removed,
    const std::vector<network::OutgoingDatagram>& datagrams);

/** @returns How many Bydos @p world holds; the missiles they fire are not
 * counted. */
[[nodiscard]] std::size_t bydosIn(game::World& world);

/**
 * @brief A game whose datagrams are recorded and whose input is queued. Every
 * player that joined sends a Heartbeat before each tick, as a client that is
 * still there sends at least one datagram per second, until silence() stops
 * it.
 */
class GameInstanceTest : public ::testing::Test {
 public:
  GameInstanceTest();

 protected:
  /**
   * @brief Connects the player number @p index + 1, under @p sessionToken, and
   * hands it to the game.
   */
  ConnectedPlayer& join(std::size_t index,
                        std::uint64_t sessionToken = SESSION_TOKEN);

  /**
   * @brief The player at @p index sends nothing from now on, as a client that
   * crashed or lost the network.
   */
  void silence(std::size_t index);

  /**
   * @brief The player at @p index sends a Disconnection for the next tick,
   * then nothing more, as a client that closes.
   */
  void leave(std::size_t index);

  /** @brief Queues a Disconnection from the player at @p index for the next
   * tick, as one of the copies its client sends. */
  void queueDisconnectionFrom(std::size_t index);

  /**
   * @returns The timers the game plans the later copies of its events on:
   * advancing them sends those copies.
   */
  [[nodiscard]] engine::TimerScheduler& timers();

  /** @returns Every datagram the game sent, oldest first. */
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sentDatagrams()
      const;

  /** @returns The PlayerLeft that @p datagrams carry to the player at
   * @p index, in the order they were sent. */
  [[nodiscard]] static std::vector<network::PlayerLeft> playerLeftSentTo(
      const std::vector<network::OutgoingDatagram>& datagrams,
      std::size_t index);

  /** @brief Queues the ReadyState described by @p sent for the next tick. */
  void sendReady(const SentReadyState& sent);

  /** @brief Queues @p datagram for the next tick. */
  void queue(network::IncomingDatagram datagram);

  /** @brief Runs one tick with what was queued, then a Heartbeat from each
   * player not silenced, then empties the queue. */
  void tick();

  void runTicks(std::size_t count);

  /** @brief Connects the players at @p indexes, who all say ready, and runs
   * the tick that starts the game. */
  void startGameWith(std::initializer_list<std::size_t> indexes);

  /** @brief Connects one player, who says ready, and runs the tick that starts
   * the game. */
  void startGameWithOnePlayer();

  /**
   * @brief Silences the player at @p index, then runs ticks until its session
   * closed: the timeout of the protocol and one tick more, which a player
   * silenced right after it joined needs, since it was never heard in a tick.
   */
  void timeOut(std::size_t index);

  /** @returns The network id of the ship of the player at @p index, or
   * NO_ENTITY_ID when that player is not in the game. */
  [[nodiscard]] std::uint32_t shipOf(std::size_t index);

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const;

  [[nodiscard]] GameInstance& game();

  /** @returns The world of the game, which the rules of R-Type own. A new
   * one replaces it when the game waits for players again. */
  [[nodiscard]] game::World& world();
  [[nodiscard]] engine::EventBus& events();

 private:
  std::vector<network::OutgoingDatagram> sent_;
  /** @brief The indexes of the players the game hears from before each tick:
   * the fixture marks them as heard from, as the Heartbeat of a client would,
   * without a datagram whose sequence would have to follow the others. */
  std::vector<std::size_t> livePlayers_;
  engine::EventBus events_;
  engine::TimerScheduler timers_;
  RtypeRulesModule* rules_{nullptr};
  GameInstance game_;
  std::vector<network::IncomingDatagram> incoming_;
};

/**
 * @brief A game with two players, each with its own session token, and the
 * network in between. Each player numbers its inputs from 1 in the order it
 * sends them, as a client does.
 */
class GameInstanceInputTest : public GameInstanceTest {
 protected:
  GameInstanceInputTest();

  /**
   * @returns The datagram of the next input of @p player, holding @p actions,
   * under @p header.
   */
  std::vector<std::byte> nextInputDatagram(std::size_t player,
                                           std::uint8_t actions,
                                           const network::PacketHeader& header);

  /** @brief @p player sends its next input, holding @p actions. */
  void send(std::size_t player, std::uint8_t actions);

  /**
   * @brief @p player sends its next input, holding @p actions, under
   * @p header.
   */
  void send(std::size_t player, std::uint8_t actions,
            const network::PacketHeader& header);

  /** @brief @p datagram leaves the address and port of @p player. */
  void sendDatagram(std::size_t player, std::vector<std::byte> datagram);

  /** @brief One tick reads the datagrams sent at @p sendIndexes. */
  void deliver(std::initializer_list<std::size_t> sendIndexes);

  /**
   * @brief Ticks read @p arrivals in order, DATAGRAMS_PER_TICK at a time.
   */
  void deliverInTicks(std::span<const network::IncomingDatagram> arrivals);

  /** @returns The number of the last input @p player sent. */
  [[nodiscard]] std::uint32_t lastInputNumber(std::size_t player) const;

  /** @returns The input the game keeps for @p player, if any. */
  [[nodiscard]] std::optional<network::PlayerInput> newestInputOf(
      std::size_t player);

  /** @returns The actions of the input the game keeps for @p player. */
  [[nodiscard]] std::optional<std::uint8_t> heldActions(std::size_t player);

  [[nodiscard]] const UnreliableNetwork& network() const;

 private:
  UnreliableNetwork network_;
  std::array<std::uint32_t, PLAYER_SESSION_TOKENS.size()> inputsSent_{};
};

/**
 * @brief The input fixture, with both players ready and a way to read where
 * their ships are.
 */
class GameInstanceMovementTest : public GameInstanceInputTest {
 protected:
  /** @brief Both players say ready, then the tick that starts the game runs. */
  void startGame();

  /** @returns The network id of the ship of @p player; NO_ENTITY_ID, after a
   * failure, when the player is not in the roster. */
  [[nodiscard]] std::uint32_t shipIdOf(std::size_t player);

  /** @returns Where the ship of @p player is; the origin, after a failure, when
   * it has none. */
  [[nodiscard]] game::Position shipPositionOf(std::size_t player);
};

/** @brief An arrow a player holds alone, named after it, as its bit of the
 * actions byte, and which way it moves a ship along each axis: -1, 0 or 1. */
struct HeldArrow {
  std::string_view name;
  std::uint8_t actions{};
  float towardsX{};
  float towardsY{};
};

/** @brief Prints an arrow by its name, so the name of each test says which
 * arrow it holds. */
void PrintTo(const HeldArrow& arrow, std::ostream* output);

/** @brief The movement fixture, run once for each arrow. */
class GameInstanceArrowTest : public GameInstanceMovementTest,
                              public ::testing::WithParamInterface<HeldArrow> {
};

/** @brief The input fixture, with the log written to a journal the test reads.
 */
class GameInstanceLogTest : public GameInstanceInputTest {
 protected:
  /** @brief An input from an address that is not a player's arrives. */
  void queueStrangersInput();

  /** @returns The lines of the journal about a datagram dropped. */
  [[nodiscard]] std::vector<std::string> dropLines() const;

  [[nodiscard]] const DebugJournal& journal() const;

 private:
  DebugJournal journal_;
};

/** @brief The input fixture, once for each seed of a shuffled delivery. */
class GameInstanceInputShuffleTest
    : public GameInstanceInputTest,
      public ::testing::WithParamInterface<std::uint32_t> {};

/** @returns The type of the only message of each of @p datagrams, in order. */
[[nodiscard]] std::vector<network::MessageType> messageTypesOf(
    const std::vector<network::OutgoingDatagram>& datagrams);

/** @returns The entity of @p state whose id is @p entityId, or nothing when the
 * state does not list it. */
[[nodiscard]] std::optional<network::EntityState> entityOf(
    const network::WorldState& state, std::uint32_t entityId);

/** @returns The room of a game whose only player, the first, is not ready. */
[[nodiscard]] network::LobbyState roomOfTheFirstPlayerNotReady();

/**
 * @returns The payload of @p input written field by field, so it may hold
 * actions encodePayload() refuses.
 */
[[nodiscard]] std::vector<std::byte> inputPayload(
    const network::PlayerInput& input);

}  // namespace rtype::server::testing
