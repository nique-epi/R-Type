#include "ServerInformationDesk.hpp"
#include <algorithm>
#include <chrono>
#include <exception>
#include <optional>
#include <span>
#include <utility>
#include "DatagramCodec.hpp"
#include "IncomingDatagram.hpp"
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
    const ServerOccupancy& occupancy, std::chrono::nanoseconds now) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    try {
      if (const std::optional<network::ServerInformationRequest> request =
              requests_.read(datagram, now)) {
        parts_.send(network::OutgoingDatagram{
            .destination = datagram.sender,
            .payload = network::DatagramCodec::encode(
                network::PacketHeader{},
                informationFor(*request, now - datagram.receivedAt,
                               occupancy))});
      }
    } catch (const std::exception& error) {
      failureLog_.error(now, "dropped a datagram from ",
                        datagram.sender.address, " port ", datagram.sender.port,
                        ": answering it threw: ", error.what());
    }
  }
}

network::ServerInformation ServerInformationDesk::informationFor(
    const network::ServerInformationRequest& request,
    std::chrono::nanoseconds held, const ServerOccupancy& occupancy) const {
  return network::ServerInformation{
      .sentAt = request.sentAt,
      .heldFor = network::wireMicrosecondsOf(
          std::max(held, std::chrono::nanoseconds::zero())),
      .protocolVersion = network::PROTOCOL_VERSION,
      .gameVersion = parts_.gameVersion,
      .name = parts_.name,
      .playerCount = occupancy.playerCount,
      .gameCount = occupancy.gameCount,
      .freeSeatCount = occupancy.freeSeatCount};
}

}  // namespace rtype::server
