#pragma once

#include "DatagramRouter.hpp"
#include "EntityMessageQueue.hpp"
#include "GameLoadedReminder.hpp"
#include "MessageDispatcher.hpp"
#include "NetworkConditions.hpp"
#include "NetworkThread.hpp"
#include "ServerSession.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The connection of one player to a server: the UDP socket on its
 * NetworkThread, a DatagramRouter in front of it, and the three parts the
 * screens and the game use, the ServerSession (the attempt and the session),
 * the WaitingRoom and the EntityMessageQueue. ClientConnectionModule holds the
 * one of the client; each load-test bot holds its own.
 *
 * Building it opens nothing: open() binds the socket on every IPv4 interface,
 * on a port the system picks, and starts the network thread. leave() tells the
 * server the player leaves and close() closes the socket; destroying it closes
 * the socket without telling the server.
 */
class PlayerConnection {
 public:
  /** @param conditions The network the datagrams of the player cross. */
  explicit PlayerConnection(const network::NetworkConditions& conditions);

  /** @throws SocketOpenException when the system refuses to open the
   * socket. */
  void open();

  /**
   * @brief Counts @p elapsed against the memory of the game events, then
   * reads what the server sent and sends what is due (see
   * ServerSession::update()).
   */
  void update(engine::Duration elapsed);

  /**
   * @brief Leaves the session with the server, if any, and waits until every
   * copy of the Disconnection went out: after each wait of
   * DISCONNECTION_COPY_INTERVAL, the session moves by that interval and sends
   * the next copy, so the server frees the place of the player at once.
   */
  void leave();

  /** @brief Stops the network thread and closes the socket (see
   * NetworkThread::close()). */
  void close();

  [[nodiscard]] ServerSession& session();
  [[nodiscard]] const ServerSession& session() const;
  [[nodiscard]] WaitingRoom& waitingRoom();
  [[nodiscard]] const WaitingRoom& waitingRoom() const;
  [[nodiscard]] EntityMessageQueue& entityMessages();

 private:
  NetworkThread network_;
  DatagramRouter router_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  WaitingRoom waitingRoom_;
  EntityMessageQueue entityMessages_;
  ServerSession session_;
};

}  // namespace rtype::client
