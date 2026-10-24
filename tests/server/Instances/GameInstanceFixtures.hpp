#pragma once

#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DebugJournal.hpp"
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameInstance.hpp"
#include "GameInstanceTestConstants.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "Position.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimerScheduler.hpp"
#include "UnreliableNetwork.hpp"

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
   * @returns A broadcaster of the game events, wired to the roster as the
   * server wires it, that runs its later copies on @p timers and puts each
   * datagram in @p datagrams.
   */
  [[nodiscard]] std::unique_ptr<GameEventBroadcaster> eventBroadcaster(
      engine::TimerScheduler& timers,
      std::vector<network::OutgoingDatagram>& datagrams);

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
  [[nodiscard]] engine::EventBus& events();

 private:
  /** @brief A player that sends a Heartbeat before each tick. */
  struct LivePlayer {
    std::size_t index{};
    std::uint64_t sessionToken{};
  };

  std::vector<network::OutgoingDatagram> sent_;
  std::vector<LivePlayer> livePlayers_;
  engine::EventBus events_;
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

}  // namespace rtype::server::testing
