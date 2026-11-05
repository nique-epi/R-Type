#include "RoundTripWatch.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include "ClockSample.hpp"
#include "Heartbeat.hpp"
#include "IClock.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "ProtocolConstants.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripMeter.hpp"
#include "RoundTripWire.hpp"
#include "RoundTrips.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

RoundTripWatch::RoundTripWatch(network::MessageDispatcher& dispatcher,
                               const engine::IClock& clock)
    : clock_(&clock) {
  dispatcher.registerHandler<network::Heartbeat>(
      [this](const network::MessageOrigin& origin,
             const network::Heartbeat& heartbeat) {
        if (outbox_ != nullptr) {
          follow(origin, heartbeat);
        }
      });
  dispatcher.registerHandler<network::RoundTrips>(
      [this](const network::MessageOrigin&, const network::RoundTrips& report) {
        if (outbox_ != nullptr) {
          reported_ = report.players;
        }
      });
}

void RoundTripWatch::sessionOpened(const network::LobbyPlayer& ownPlayer,
                                   ISessionOutbox& outbox) {
  outbox_ = &outbox;
  ownPlayerNumber_ = ownPlayer.playerNumber;
  sinceHeartbeat_ = engine::Duration::zero();
}

void RoundTripWatch::sessionClosed() {
  outbox_ = nullptr;
  meter_ = network::RoundTripMeter{};
  serverClock_.reset();
  reported_.clear();
}

void RoundTripWatch::sessionHeld(engine::Duration elapsed) {
  sinceHeartbeat_ += elapsed;
  if (outbox_ == nullptr || (sinceHeartbeat_ < network::HEARTBEAT_INTERVAL &&
                             !outbox_->isAcknowledgementDue())) {
    return;
  }
  sinceHeartbeat_ = engine::Duration::zero();
  sendMessage(*outbox_, meter_.nextHeartbeat(clock_->now()));
}

const engine::IServerClock& RoundTripWatch::serverClock() const {
  return serverClock_;
}

void RoundTripWatch::follow(const network::MessageOrigin& origin,
                            const network::Heartbeat& heartbeat) {
  const std::optional<std::chrono::microseconds> roundTrip =
      meter_.receive(heartbeat, origin.receivedAt);
  if (roundTrip.has_value() && heartbeat.tick.has_value()) {
    serverClock_.add(engine::ClockSample{.receivedAt = origin.receivedAt,
                                         .roundTrip = *roundTrip,
                                         .tick = *heartbeat.tick});
  }
}

std::optional<network::RoundTripFigures> RoundTripWatch::roundTripOf(
    std::uint8_t playerNumber) const {
  if (outbox_ == nullptr) {
    return std::nullopt;
  }
  if (playerNumber == ownPlayerNumber_) {
    return meter_.figures();
  }
  return reportedRoundTripOf(playerNumber);
}

std::optional<network::RoundTripFigures> RoundTripWatch::reportedRoundTripOf(
    std::uint8_t playerNumber) const {
  const auto reported = std::ranges::find(
      reported_, playerNumber, &network::PlayerRoundTrip::playerNumber);
  if (reported == reported_.end()) {
    return std::nullopt;
  }
  return network::figuresOf(*reported);
}

}  // namespace rtype::client
