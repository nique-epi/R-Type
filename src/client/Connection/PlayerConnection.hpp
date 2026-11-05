#pragma once

#include "DatagramRouter.hpp"
#include "EntityMessageQueue.hpp"
#include "GameLoadedReminder.hpp"
#include "MessageDispatcher.hpp"
#include "NetworkConditions.hpp"
#include "NetworkThread.hpp"
#include "OrderedInbox.hpp"
#include "RoundTripWatch.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerSession.hpp"
#include "SystemClock.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The connection of one player to a server: the UDP socket on its
 * NetworkThread, a DatagramRouter in front of it, and the parts the screens
 * and the game use, the ServerSession (the attempt and the session), the
 * WaitingRoom, the EntityMessageQueue, the ServerLeaderboard and the
 * RoundTripWatch, which sends the heartbeats and measures the round trips.
 * ClientConnectionModule holds the one of the client; each load-test bot holds
 * its own.
 *
 * Building it opens nothing: network().open() binds the socket on every IPv4
 * interface, on a port the system picks, and starts the network thread.
 * leave() tells the server the player leaves and network().close() closes the
 * socket; destroying it closes the socket without telling the server.
 */
class PlayerConnection {
 public:
  /** @param conditions The network the datagrams of the player cross. */
  explicit PlayerConnection(const network::NetworkConditions& conditions);

  /**
   * @brief Reads what the server sent and sends what is due (see
   * ServerSession::update()).
   */
  void update(engine::Duration elapsed);

  /**
   * @brief Leaves the session with the server, if any, and waits until every
   * copy of the Disconnection went out: after each wait of
   * DISCONNECTION_COPY_INTERVAL, the session moves by that interval and sends
   * the next copy. The server frees the place of the player as soon as one
   * copy reaches it. When none does, lost on the way, or still in flight over
   * a simulated network when the socket closes (see NetworkThread::close()),
   * it frees the place once the player was silent for SESSION_SILENCE_LIMIT,
   * counted in the ticks it runs, so longer when the server stalls.
   */
  void leave();

  /** @returns The socket on its network thread: open() it before the first
   * update(), close() it after leave(), read its traffic(). */
  [[nodiscard]] NetworkThread& network();

  [[nodiscard]] ServerSession& session();
  [[nodiscard]] const ServerSession& session() const;
  [[nodiscard]] WaitingRoom& waitingRoom();
  [[nodiscard]] const WaitingRoom& waitingRoom() const;
  [[nodiscard]] EntityMessageQueue& entityMessages();
  [[nodiscard]] ServerLeaderboard& leaderboard();
  [[nodiscard]] RoundTripWatch& roundTrips();

 private:
  NetworkThread network_;
  DatagramRouter router_;
  network::MessageDispatcher dispatcher_;
  OrderedInbox orderedInbox_;
  GameLoadedReminder gameLoaded_;
  WaitingRoom waitingRoom_;
  EntityMessageQueue entityMessages_;
  engine::SystemClock clock_;
  RoundTripWatch roundTrips_;
  ServerLeaderboard leaderboard_;
  ServerSession session_;
};

}  // namespace rtype::client
