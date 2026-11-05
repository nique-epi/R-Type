#include "ConnectionAcceptorFixtures.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <vector>
#include "ByteWriter.hpp"
#include "DatagramCodec.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

std::vector<std::byte> emptyNicknameRequestDatagram() {
  network::ByteWriter payload;
  payload.writeUint64(0);
  payload.writeString("");
  return network::DatagramCodec::encode(network::PacketHeader{},
                                        network::MessageType::ConnectionRequest,
                                        payload.bytes());
}

std::string nicknameOf(std::size_t player) {
  return std::string{NICKNAME_PREFIX} + std::to_string(player);
}

std::size_t acceptancesAmong(
    std::span<const network::OutgoingDatagram> answers) {
  return static_cast<std::size_t>(std::ranges::count_if(
      answers, [](const network::OutgoingDatagram& answer) {
        return onlyMessageTypeOf(answer) ==
               network::MessageType::ConnectionAccepted;
      }));
}

}  // namespace rtype::server::testing
