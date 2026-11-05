#include "ServerInformationDesk.hpp"
#include <algorithm>
#include <chrono>
#include <exception>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "InformationConstants.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RoundTripWire.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationDeskParts.hpp"
#include "ServerInformationRequest.hpp"
#include "ServerOccupancy.hpp"

namespace rtype::server {

ServerInformationDesk::ServerInformationDesk(ServerInformationDeskParts parts)
    : parts_(std::move(parts)) {}

void ServerInformationDesk::answer(
    std::span<const network::IncomingDatagram> datagrams,
    std::chrono::nanoseconds now) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    try {
      if (const std::optional<network::ServerInformationRequest> request =
              readRequest(datagram, now)) {
        parts_.send(network::OutgoingDatagram{
            .destination = datagram.sender,
            .payload = network::DatagramCodec::encode(
                network::PacketHeader{},
                informationFor(*request, datagram.receivedAt, now))});
      }
    } catch (const std::exception& error) {
      failureLog_.error(now, "dropped a datagram from ",
                        datagram.sender.address, " port ", datagram.sender.port,
                        ": answering it threw: ", error.what());
    }
  }
}

std::optional<network::ServerInformationRequest>
ServerInformationDesk::readRequest(const network::IncomingDatagram& datagram,
                                   std::chrono::nanoseconds now) {
  std::optional<network::ServerInformationRequest> request;
  try {
    request =
        network::DatagramCodec::firstMessage<network::ServerInformationRequest>(
            datagram.payload);
  } catch (const network::NetworkException& error) {
    drop(datagram.sender, error.what(), now);
    return std::nullopt;
  }
  if (!request.has_value()) {
    drop(datagram.sender, NO_INFORMATION_REQUEST_DROP_REASON, now);
  }
  return request;
}

network::ServerInformation ServerInformationDesk::informationFor(
    const network::ServerInformationRequest& request,
    std::chrono::nanoseconds receivedAt, std::chrono::nanoseconds now) const {
  const ServerOccupancy occupancy = parts_.occupancy();
  const std::chrono::nanoseconds held =
      std::max(now - receivedAt, std::chrono::nanoseconds::zero());
  return network::ServerInformation{
      .sentAt = request.sentAt,
      .heldFor = network::wireMicrosecondsOf(held),
      .protocolVersion = network::PROTOCOL_VERSION,
      .gameVersion = parts_.gameVersion,
      .name = parts_.name,
      .playerCount = occupancy.playerCount,
      .gameCount = occupancy.gameCount,
      .freeSeatCount = occupancy.freeSeatCount};
}

void ServerInformationDesk::drop(const network::Endpoint& sender,
                                 std::string_view reason,
                                 std::chrono::nanoseconds now) {
  dropLog_.debug(now, "dropped a datagram from ", sender.address, " port ",
                 sender.port, ": ", reason);
}

}  // namespace rtype::server
