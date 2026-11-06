#include "GameChatFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "ChatLine.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "GameChatTestConstants.hpp"
#include "GameInstanceFixtures.hpp"
#include "IncomingDatagram.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

network::IncomingDatagram chatLineFrom(std::size_t index) {
  return network::IncomingDatagram{
      .sender = playerEndpoint(index),
      .payload = network::DatagramCodec::encode(
          firstSessionHeader(index),
          network::ChatLine{.lineNumber = CHAT_LINE_NUMBER,
                            .text = std::string{CHAT_TEXT}})};
}

network::IncomingDatagram disconnectionThenChatLineFrom(std::size_t index) {
  std::vector<std::byte> datagram = network::DatagramCodec::encode(
      firstSessionHeader(index), network::Disconnection{});
  const std::vector<std::byte> line = chatLineFrom(index).payload;
  datagram.insert(datagram.end(), line.begin() + network::PACKET_HEADER_SIZE,
                  line.end());
  return network::IncomingDatagram{.sender = playerEndpoint(index),
                                   .payload = datagram};
}

}  // namespace rtype::server::testing
