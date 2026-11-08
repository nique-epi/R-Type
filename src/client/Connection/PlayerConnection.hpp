#pragma once

#include "ArrivalTally.hpp"
#include "ChatChannel.hpp"
#include "ContentLoading.hpp"
#include "ContentPlaces.hpp"
#include "DatagramRouter.hpp"
#include "EntityMessageQueue.hpp"
#include "GameLoadedReminder.hpp"
#include "IServerPingProbe.hpp"
#include "InstallContext.hpp"
#include "MessageDispatcher.hpp"
#include "NetworkConditions.hpp"
#include "NetworkThread.hpp"
#include "OrderedInbox.hpp"
#include "RoundTripWatch.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerPingProbe.hpp"
#include "ServerSession.hpp"
#include "SessionEntityTypes.hpp"
#include "ShipLookExchange.hpp"
#include "SystemClock.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The connection of one player to a server: the UDP socket on its
 * NetworkThread, a DatagramRouter in front of it, and the parts the screens
 * and the game use, the ServerSession (the attempt and the session), the
 * WaitingRoom, the EntityMessageQueue, the ServerLeaderboard, the
 * ChatChannel, the RoundTripWatch, which sends the heartbeats and measures the
 * round trips, the SessionEntityTypes, the entity types of the content the
 * session plays with, the ContentLoading of the packs of servers, and the
 * ServerPingProbe of the list of servers, on a route of its own.
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
  /**
   * @param conditions The network the datagrams of the player cross.
   * @param places The assets folder whose content is the player's own, read
   * at once, and the folder the packs of servers are kept in.
   * @throws What localContentOf() throws, when the assets cannot make a pack.
   */
  PlayerConnection(const network::NetworkConditions& conditions,
                   const ContentPlaces& places);

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

  /**
   * @brief Provides to the modules installed after the one installing it the
   * parts of this connection: the ServerSession, the WaitingRoom, the
   * ShipLookExchange, the EntityMessageQueue, the ServerLeaderboard, the
   * ChatChannel, the RoundTripWatch as IPlayerRoundTrips, IServerTicks and
   * IOwnRoundTrip,
   * the ArrivalTally of the session as ISessionArrivals,
   * the IEntityTypeTable and the TrafficCounters of the socket. They must
   * outlive every module that requires them. The ServerPingProbe is reached
   * through pings(). The client's own content is drawn with the
   * EntityAppearances of the assets module from then on.
   * @throws MissingServiceException when no module provided the
   * EntityAppearances.
   */
  void provideParts(engine::InstallContext& context);

  /** @returns The socket on its network thread: open() it before the first
   * update(), close() it after leave(), read its traffic(). */
  [[nodiscard]] NetworkThread& network();

  [[nodiscard]] ServerSession& session();
  [[nodiscard]] const ServerSession& session() const;
  [[nodiscard]] WaitingRoom& waitingRoom();
  [[nodiscard]] const WaitingRoom& waitingRoom() const;
  [[nodiscard]] EntityMessageQueue& entityMessages();

  /** @returns The probe of the list of servers, on its own route. */
  [[nodiscard]] IServerPingProbe& pings();

 private:
  NetworkThread network_;
  DatagramRouter router_;
  network::MessageDispatcher dispatcher_;
  OrderedInbox orderedInbox_;
  GameLoadedReminder gameLoaded_;
  WaitingRoom waitingRoom_;
  ShipLookExchange shipLooks_;
  EntityMessageQueue entityMessages_;
  engine::SystemClock clock_;
  RoundTripWatch roundTrips_;
  ServerLeaderboard leaderboard_;
  ChatChannel chat_;
  SessionEntityTypes entityTypes_;
  ContentLoading content_;
  ArrivalTally arrivals_;
  ServerSession session_;
  ServerPingProbe pings_;
};

}  // namespace rtype::client
