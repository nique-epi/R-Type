#include "ConnectionAcceptorFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "ByteWriter.hpp"
#include "DatagramCodec.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
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

}  // namespace rtype::server::testing
