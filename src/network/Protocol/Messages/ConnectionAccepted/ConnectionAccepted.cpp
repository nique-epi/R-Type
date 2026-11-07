#include "ConnectionAccepted.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "ContentAnnouncement.hpp"
#include "ContentPackConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const ConnectionAccepted& message) {
  requireValidPlayerNumber(message.playerNumber);
  if (message.content.size > MAXIMUM_CONTENT_PACK_BYTES) {
    throw InvalidFieldValueException(CONTENT_SIZE_FIELD, message.content.size);
  }
}

ContentAnnouncement readContent(ByteReader& reader) {
  ContentAnnouncement content;
  const std::span<const std::byte> fingerprint =
      reader.readBytes(content.fingerprint.bytes.size());
  std::ranges::copy(fingerprint, content.fingerprint.bytes.begin());
  content.size = reader.readUint32();
  content.port = reader.readUint16();
  return content;
}

}  // namespace

void encodePayload(ByteWriter& writer, const ConnectionAccepted& message) {
  validate(message);
  writer.writeUint8(message.playerNumber);
  writer.writeBytes(message.content.fingerprint.bytes);
  writer.writeUint32(message.content.size);
  writer.writeUint16(message.content.port);
}

ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionAccepted> type) {
  ByteReader reader{payload};
  ConnectionAccepted message{.playerNumber = reader.readUint8(),
                             .content = readContent(reader)};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
