#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "Heartbeat.hpp"
#include "IClock.hpp"
#include "IPlayerRoundTrips.hpp"
#include "IServerClock.hpp"
#include "IServerTicks.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripMeter.hpp"
#include "RoundTrips.hpp"
#include "ServerClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The round trip between the client and its server, the round trips
 * the server measured with the other players, and the clock of the server
 * seen from this client.
 *
 * While a session is open, it sends the server a Heartbeat every
 * HEARTBEAT_INTERVAL, counted on the frames the session holds, whatever else
 * the client sends, and one at once when the outbox says an acknowledgement
 * is due (see ISessionOutbox::isAcknowledgementDue()): its RoundTripMeter
 * builds it at the time on the clock and
 * echoes the newest Heartbeat of the server. Each Heartbeat of the server goes
 * to that meter with the time its datagram arrived, and each RoundTrips
 * replaces the round trips of the others. A Heartbeat of the server that
 * carries a tick and closes a measured round trip also gives the ServerClock
 * a sample: the server was at that tick half a round trip before the
 * datagram arrived. All of it is forgotten when the session closes, and what
 * arrives without a session is ignored.
 *
 * Not thread-safe: use it from the frame loop.
 */
class RoundTripWatch final : public ISessionObserver,
                             public IPlayerRoundTrips,
                             public IServerTicks {
 public:
  /**
   * @param dispatcher Where it registers the handlers of Heartbeat and
   * RoundTrips; it must outlive the watch.
   * @param clock Read when a Heartbeat is built: the steady clock the network
   * thread stamps each datagram with (see IncomingDatagram::receivedAt); it
   * must outlive the watch.
   */
  RoundTripWatch(network::MessageDispatcher& dispatcher,
                 const engine::IClock& clock);

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;
  void sessionClosed() override;
  /** @brief Sends a Heartbeat once HEARTBEAT_INTERVAL passed since the
   * previous one, or since the session opened, or once an acknowledgement is
   * due; the interval then starts over. */
  void sessionHeld(engine::Duration elapsed) override;
  [[nodiscard]] std::optional<network::RoundTripFigures> roundTripOf(
      std::uint8_t playerNumber) const override;

  /** @returns Which tick the server is at on the clock of this client, and
   * the instant of a tick; both empty until a Heartbeat of the server in a
   * game closed a measured round trip, and again once the session closes. */
  [[nodiscard]] const engine::IServerClock& serverClock() const;

  /** @returns The tick of the server at the time on the clock given at
   * construction, as serverClock() places it; nothing while that is empty. */
  [[nodiscard]] std::optional<double> serverTickNow() const override;

 private:
  /** @brief Hands @p heartbeat to the meter, and its tick to the ServerClock
   * when the datagram gave a round trip. Only a session that passed the checks
   * of its datagrams reaches it, so a forged Heartbeat cannot move the clock.
   */
  void follow(const network::MessageOrigin& origin,
              const network::Heartbeat& heartbeat);

  /** @returns The round trip the newest RoundTrips lists for @p playerNumber.
   */
  [[nodiscard]] std::optional<network::RoundTripFigures> reportedRoundTripOf(
      std::uint8_t playerNumber) const;

  const engine::IClock* clock_;
  ISessionOutbox* outbox_{nullptr};
  std::uint8_t ownPlayerNumber_{};
  engine::Duration sinceHeartbeat_{};
  network::RoundTripMeter meter_;
  engine::ServerClock serverClock_;
  std::vector<network::PlayerRoundTrip> reported_;
};

}  // namespace rtype::client
