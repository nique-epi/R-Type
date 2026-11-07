#include "ChatChannelFixture.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "ChatChannel.hpp"
#include "ChatLine.hpp"
#include "ConnectionAccepted.hpp"
#include "DatagramCodec.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerChatLine.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

void ChatChannelFixture::connectAndGetAccepted() {
  session_.connect(server(), NICKNAME, rtype::network::ANY_GAME_CHOICE);
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          header,
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .content = localAnnouncement()})});
  update();
}

void ChatChannelFixture::update(engine::Duration elapsed) {
  session_.update(elapsed);
}

void ChatChannelFixture::relayLine(std::uint16_t lineNumber,
                                   std::string_view text,
                                   std::uint8_t playerNumber) {
  const network::PlayerChatLine relayed{
      .line = {.lineNumber = lineNumber, .text = std::string{text}},
      .playerNumber = playerNumber};
  link_.deliver(
      network::IncomingDatagram{.sender = server(),
                                .payload = network::DatagramCodec::encode(
                                    sessionHeader(nextSequence_), relayed)});
  ++nextSequence_;
  update();
}

ChatChannel& ChatChannelFixture::chat() { return chat_; }

RecordingWaitingRoom& ChatChannelFixture::room() { return room_; }

std::vector<network::ChatLine> ChatChannelFixture::sentLines() const {
  std::vector<network::ChatLine> lines;
  for (const network::OutgoingDatagram& datagram : link_.sent()) {
    if (holds<network::ChatLine>(datagram)) {
      lines.push_back(onlyMessageOf<network::ChatLine>(datagram));
    }
  }
  return lines;
}

}  // namespace rtype::client::testing
