#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "IClock.hpp"
#include "IPlayerRoundTrips.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripMeter.hpp"
#include "RoundTrips.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The round trip between the client and its server, and the round
 * trips the server measured with the other players.
 *
 * While a session is open, it sends the server a Heartbeat every
 * HEARTBEAT_INTERVAL, counted on the frames the session holds, whatever else
 * the client sends, and one at once when the outbox says an acknowledgement
 * is due (see ISessionOutbox::isAcknowledgementDue()): its RoundTripMeter
 * builds it at the time on the clock and
 * echoes the newest Heartbeat of the server. Each Heartbeat of the server goes
 * to that meter with the time its datagram arrived, and each RoundTrips
 * replaces the round trips of the others. All of it is forgotten when the
 * session closes, and what arrives without a session is ignored.
 *
 * Not thread-safe: use it from the frame loop.
 */
class RoundTripWatch final : public ISessionObserver, public IPlayerRoundTrips {
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

 private:
  /** @returns The round trip the newest RoundTrips lists for @p playerNumber.
   */
  [[nodiscard]] std::optional<network::RoundTripFigures> reportedRoundTripOf(
      std::uint8_t playerNumber) const;

  const engine::IClock* clock_;
  ISessionOutbox* outbox_{nullptr};
  std::uint8_t ownPlayerNumber_{};
  engine::Duration sinceHeartbeat_{};
  network::RoundTripMeter meter_;
  std::vector<network::PlayerRoundTrip> reported_;
};

}  // namespace rtype::client
