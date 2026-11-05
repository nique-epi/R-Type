#include "RequestReading.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

std::optional<network::ConnectionRequest> firstConnectionRequest(
    std::span<const std::byte> datagram) {
  if (!network::DatagramCodec::speaksThisProtocol(
          network::DatagramCodec::decodeHeader(datagram))) {
    return std::nullopt;
  }
  for (const network::MessageFrame& frame :
       network::DatagramCodec::splitMessages(datagram)) {
    if (frame.type ==
        static_cast<std::uint8_t>(network::ConnectionRequest::TYPE)) {
      return network::decodeMessage<network::ConnectionRequest>(frame.payload);
    }
  }
  return std::nullopt;
}

bool isOfAnotherVersion(std::span<const std::byte> datagram) {
  return datagram.size() >= network::PACKET_HEADER_SIZE &&
         network::DatagramCodec::speaksAnotherVersion(
             network::DatagramCodec::decodeHeader(datagram));
}

}  // namespace rtype::server
