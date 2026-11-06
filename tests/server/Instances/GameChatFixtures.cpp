#include "GameChatFixtures.hpp"
#include <cstddef>
#include <string>
#include "ChatLine.hpp"
#include "DatagramCodec.hpp"
#include "GameChatTestConstants.hpp"
#include "GameInstanceFixtures.hpp"
#include "IncomingDatagram.hpp"
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
  return disconnectionThen(index, chatLineFrom(index));
}

}  // namespace rtype::server::testing
