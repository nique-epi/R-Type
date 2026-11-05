#include "ServerPingProbe.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IRoutedLink.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PingAnswer.hpp"
#include "RoundTripWire.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationRequest.hpp"

namespace rtype::client {

ServerPingProbe::ServerPingProbe(IRoutedLink& link, const engine::IClock& clock)
    : link_(&link), clock_(&clock) {}

void ServerPingProbe::measure(std::vector<network::Endpoint> servers) {
  link_->listenToEach(servers);
  const network::ServerInformationRequest request =
      network::informationRequestAt(clock_->now());
  const std::vector<std::byte> payload =
      network::DatagramCodec::encode(network::PacketHeader{}, request);
  for (const network::Endpoint& server : servers) {
    link_->send(
        network::OutgoingDatagram{.destination = server, .payload = payload});
  }
  sentAt_ = request.sentAt;
  unanswered_ = std::move(servers);
}

std::vector<PingAnswer> ServerPingProbe::answers() {
  received_.clear();
  link_->drainReceived(received_);
  std::vector<PingAnswer> answers;
  for (const network::IncomingDatagram& datagram : received_) {
    if (const std::optional<network::ServerInformation> information =
            informationIn(datagram)) {
      if (std::optional<PingAnswer> answer = answerOf(datagram, *information)) {
        answers.push_back(std::move(*answer));
      }
    }
  }
  return answers;
}

std::optional<network::ServerInformation> ServerPingProbe::informationIn(
    const network::IncomingDatagram& datagram) const {
  if (std::ranges::find(unanswered_, datagram.sender) == unanswered_.end()) {
    return std::nullopt;
  }
  std::optional<network::ServerInformation> information;
  try {
    information =
        network::DatagramCodec::firstMessage<network::ServerInformation>(
            datagram.payload);
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
  if (!information.has_value() || information->sentAt != sentAt_) {
    return std::nullopt;
  }
  return information;
}

std::optional<PingAnswer> ServerPingProbe::answerOf(
    const network::IncomingDatagram& datagram,
    const network::ServerInformation& information) {
  const std::optional<std::chrono::microseconds> roundTrip =
      network::informationRoundTripOf(information, datagram.receivedAt);
  if (!roundTrip.has_value()) {
    return std::nullopt;
  }
  std::erase(unanswered_, datagram.sender);
  return PingAnswer{.server = datagram.sender,
                    .roundTrip = *roundTrip,
                    .information = information};
}

}  // namespace rtype::client
