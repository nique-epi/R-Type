#include "DatagramCompressionFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "CompressionFixtures.hpp"
#include "CompressionTestConstants.hpp"
#include "DatagramCodec.hpp"
#include "DatagramCompression.hpp"
#include "DatagramCompressionTestConstants.hpp"
#include "Endpoint.hpp"
#include "GameStatus.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RealWorldStateBodiesTestConstants.hpp"

namespace rtype::network::testing {

PacketHeader sessionHeader() {
  return PacketHeader{.sessionToken = SESSION_TOKEN};
}

PacketHeader handshakeHeader() { return PacketHeader{}; }

std::vector<std::byte> worldStateDatagram(const PacketHeader& header) {
  const std::vector<std::byte> body = bytesOf(MEDIUM_WORLD_STATE_BODY);
  const std::vector<std::byte> payload{
      body.begin() + static_cast<std::ptrdiff_t>(MESSAGE_PREFIX_SIZE),
      body.end()};
  return DatagramCodec::encode(header, MessageType::WorldState, payload);
}

std::vector<std::byte> gameStatusDatagram(const PacketHeader& header) {
  const std::string nickname{REPEATED_NICKNAME};
  return DatagramCodec::encode(
      header, GameStatus{.state = GameStatusState::InProgress,
                         .nicknames = {nickname, nickname, nickname, nickname},
                         .measures = {}});
}

std::vector<std::byte> incompressibleDatagram(const PacketHeader& header) {
  std::vector<std::byte> datagram = DatagramCodec::encode(
      header, MessageType::Heartbeat, std::vector<std::byte>{});
  const std::vector<std::byte> body =
      randomBytes(INCOMPRESSIBLE_BODY_SIZE, BYTES_SEED);
  datagram.insert(datagram.end(), body.begin(), body.end());
  return datagram;
}

std::vector<std::byte> codedSessionDatagram() {
  return compressedForm(worldStateDatagram(sessionHeader()))
      .value_or(std::vector<std::byte>{});
}

std::vector<std::byte> codedDatagramWithoutSession() {
  std::vector<std::byte> coded = codedSessionDatagram();
  for (std::size_t offset = TOKEN_OFFSET;
       offset < TOKEN_OFFSET + SESSION_TOKEN_SIZE; ++offset) {
    coded[offset] = std::byte{0};
  }
  return coded;
}

Endpoint peer() {
  return Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS},
                  .port = PEER_PORT};
}

}  // namespace rtype::network::testing
