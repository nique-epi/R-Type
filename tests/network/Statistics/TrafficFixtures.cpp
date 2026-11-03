#include "TrafficFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <sstream>
#include <string>
#include <vector>
#include "DatagramCodec.hpp"
#include "DirectionTraffic.hpp"
#include "Endpoint.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "TrafficFigures.hpp"

namespace rtype::network::testing {

std::vector<std::byte> datagramHolding(MessageType type,
                                       std::size_t payloadSize) {
  return DatagramCodec::encode(PacketHeader{}, type,
                               std::vector<std::byte>(payloadSize));
}

std::vector<std::byte> withMessage(std::vector<std::byte> datagram,
                                   MessageType type, std::size_t payloadSize) {
  const std::vector<std::byte> other = datagramHolding(type, payloadSize);
  const std::span<const std::byte> message =
      std::span<const std::byte>{other}.subspan(PACKET_HEADER_SIZE);
  datagram.insert(datagram.end(), message.begin(), message.end());
  return datagram;
}

Endpoint loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS}, .port = port};
}

TrafficFigures figuresOf(const DirectionTraffic& traffic, MessageType type) {
  return traffic.messages[static_cast<std::size_t>(type)];
}

std::string textOf(const std::filesystem::path& file) {
  const std::ifstream stream{file};
  std::ostringstream text;
  text << stream.rdbuf();
  return text.str();
}

}  // namespace rtype::network::testing
