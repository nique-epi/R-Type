#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <vector>
#include "BoundedQueue.hpp"
#include "ConnectedPlayer.hpp"
#include "IResultsArchive.hpp"
#include "LeaderboardReply.hpp"
#include "LeaderboardRequest.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief Takes the requests of the players of a game to the archive, and brings
 * the answers back to them.
 *
 * A player gets at most one answer per TICKS_BETWEEN_LEADERBOARD_ANSWERS ticks,
 * so a flood costs one read. The wait belongs to the player, not to its seat:
 * the next player on a seat freed by a departure does not inherit it. An answer
 * goes out only to a player still in the same session: the sorter let the
 * request through because that session proved its address, and only then may
 * the server answer with more bytes than it received. The ticks are counted
 * here because the tick of the game stays 0 while it waits for players, when a
 * player reads the leaderboard.
 *
 * Not thread-safe, the queue of answers aside: use it from the simulation
 * thread only.
 */
class LeaderboardDesk {
 public:
  /**
   * @param roster The players of the game; it must outlive the desk.
   * @param archive Reads the leaderboards; it must outlive the desk.
   * @param send Called for each answer. Must not be empty.
   */
  LeaderboardDesk(PlayerRoster& roster, IResultsArchive& archive,
                  std::function<void(network::OutgoingDatagram)> send);

  /** @brief Hands the request of @p player to the archive, unless the player
   * was answered too recently. */
  void onAsked(const ConnectedPlayer& player,
               const network::LeaderboardRequest& request);

  /** @brief Forgets the wait of @p departed, whose seat is freed. */
  void onPlayerClosing(const ConnectedPlayer& departed);

  /** @brief Counts a tick, and sends the answers that came back. */
  void onTick();

 private:
  void deliver(const LeaderboardReply& reply);

  PlayerRoster& roster_;
  IResultsArchive& archive_;
  std::function<void(network::OutgoingDatagram)> send_;
  concurrency::BoundedQueue<LeaderboardReply> replies_;
  std::vector<LeaderboardReply> repliesToSend_;
  std::array<std::size_t, network::MAXIMUM_PLAYERS> nextAnswerTicks_{};
  std::size_t ticks_{0};
};

}  // namespace rtype::server
