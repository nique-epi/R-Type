#include "ConnectionAcceptorFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "ByteWriter.hpp"
#include "DatagramCodec.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"

namespace rtype::server::testing {

std::vector<std::byte> emptyNicknameRequestDatagram() {
  network::ByteWriter payload;
  payload.writeUint64(0);
  payload.writeString("");
  return network::DatagramCodec::encode(network::PacketHeader{},
                                        network::MessageType::ConnectionRequest,
                                        payload.bytes());
}

std::string nicknameOf(std::size_t requester) {
  return "Pilot" + std::to_string(requester);
}

}  // namespace rtype::server::testing
