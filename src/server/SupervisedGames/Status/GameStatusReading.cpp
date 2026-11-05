#include "GameStatusReading.hpp"
#include <cstdint>
#include <optional>
#include "DatagramCodec.hpp"
#include "GameStatus.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ReceivedStatus.hpp"

namespace rtype::server {

std::optional<ReceivedStatus> readGameStatus(
    const network::IncomingDatagram& datagram) {
  try {
    const network::PacketHeader header =
        network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      return std::nullopt;
    }
    for (const network::MessageFrame& frame :
         network::DatagramCodec::splitMessages(datagram.payload)) {
      if (frame.type == static_cast<std::uint8_t>(network::GameStatus::TYPE)) {
        return ReceivedStatus{
            .token = header.sessionToken,
            .sequence = header.sequence,
            .status =
                network::decodeMessage<network::GameStatus>(frame.payload)};
      }
    }
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
  return std::nullopt;
}

}  // namespace rtype::server
