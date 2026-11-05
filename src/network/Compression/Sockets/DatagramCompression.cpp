#include "DatagramCompression.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include <vector>
#include "CompressionConstants.hpp"
#include "DatagramCodec.hpp"
#include "HuffmanCodec.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

namespace {

bool hasBody(std::span<const std::byte> datagram) {
  return datagram.size() > PACKET_HEADER_SIZE;
}

/**
 * @returns The header of @p datagram with its flags byte replaced, followed by
 * @p body.
 */
std::vector<std::byte> rebuilt(std::span<const std::byte> datagram,
                               std::byte flags,
                               std::span<const std::byte> body) {
  std::vector<std::byte> result;
  result.reserve(PACKET_HEADER_SIZE + body.size());
  result.insert(
      result.end(), datagram.begin(),
      datagram.begin() + static_cast<std::ptrdiff_t>(PACKET_HEADER_SIZE));
  result[FLAGS_OFFSET] = flags;
  result.insert(result.end(), body.begin(), body.end());
  return result;
}

}  // namespace

std::optional<std::vector<std::byte>> compressedForm(
    std::span<const std::byte> datagram) {
  if (!hasBody(datagram)) {
    return std::nullopt;
  }
  const PacketHeader header = DatagramCodec::decodeHeader(datagram);
  if (header.sessionToken == NO_SESSION_TOKEN ||
      (header.flags & COMPRESSED_FLAG) != 0) {
    return std::nullopt;
  }
  const std::span<const std::byte> body = datagram.subspan(PACKET_HEADER_SIZE);
  const std::vector<std::byte> coded = huffmanEncode(body);
  if (coded.size() >= body.size()) {
    return std::nullopt;
  }
  return rebuilt(datagram, datagram[FLAGS_OFFSET] | std::byte{COMPRESSED_FLAG},
                 coded);
}

bool isCompressed(std::span<const std::byte> datagram) {
  if (!hasBody(datagram)) {
    return false;
  }
  const PacketHeader header = DatagramCodec::decodeHeader(datagram);
  return header.protocolId == PROTOCOL_ID &&
         header.version == PROTOCOL_VERSION &&
         (header.flags & COMPRESSED_FLAG) != 0;
}

std::optional<std::vector<std::byte>> plainForm(
    std::span<const std::byte> datagram) {
  const PacketHeader header = DatagramCodec::decodeHeader(datagram);
  if (header.sessionToken == NO_SESSION_TOKEN) {
    return std::nullopt;
  }
  const std::optional<std::vector<std::byte>> body =
      huffmanDecode(datagram.subspan(PACKET_HEADER_SIZE), MAXIMUM_BODY_SIZE);
  if (!body.has_value()) {
    return std::nullopt;
  }
  return rebuilt(datagram, datagram[FLAGS_OFFSET] & ~std::byte{COMPRESSED_FLAG},
                 *body);
}

}  // namespace rtype::network
