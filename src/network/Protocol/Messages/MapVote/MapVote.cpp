#include "MapVote.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const MapVote& message) {
  writer.writeUint8(message.map);
}

MapVote decodePayload(std::span<const std::byte> payload,
                      [[maybe_unused]] std::type_identity<MapVote> type) {
  ByteReader reader{payload};
  const MapVote message{.map = reader.readUint8()};
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network
