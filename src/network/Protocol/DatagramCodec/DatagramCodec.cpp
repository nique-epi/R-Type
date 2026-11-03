#include "DatagramCodec.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "MessageFrame.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

namespace {

void writeHeader(ByteWriter& writer, const PacketHeader& header) {
  writer.writeUint32(header.protocolId);
  writer.writeUint8(header.version);
  writer.writeUint8(header.flags);
  writer.writeUint64(header.sessionToken);
  writer.writeUint16(header.sequence);
  writer.writeUint16(header.acknowledgedSequence);
  writer.writeUint32(header.acknowledgementBits);
}

void requireHeader(std::span<const std::byte> datagram) {
  if (datagram.size() < PACKET_HEADER_SIZE) {
    throw MalformedDatagramException(datagram.size());
  }
}

}  // namespace

std::vector<std::byte> DatagramCodec::encode(
    const PacketHeader& header, MessageType type,
    std::span<const std::byte> payload) {
  if (payload.size() > MESSAGE_PAYLOAD_LIMIT) {
    throw DatagramTooLargeException(PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE +
                                    payload.size());
  }
  ByteWriter writer;
  writeHeader(writer, header);
  writer.writeUint8(static_cast<std::uint8_t>(type));
  writer.writeUint16(static_cast<std::uint16_t>(payload.size()));
  writer.writeBytes(payload);
  return writer.bytes();
}

PacketHeader DatagramCodec::decodeHeader(std::span<const std::byte> datagram) {
  requireHeader(datagram);
  ByteReader reader{datagram};
  return PacketHeader{.protocolId = reader.readUint32(),
                      .version = reader.readUint8(),
                      .flags = reader.readUint8(),
                      .sessionToken = reader.readUint64(),
                      .sequence = reader.readUint16(),
                      .acknowledgedSequence = reader.readUint16(),
                      .acknowledgementBits = reader.readUint32()};
}

bool DatagramCodec::speaksThisProtocol(const PacketHeader& header) {
  const PacketHeader thisProtocol;
  return header.protocolId == thisProtocol.protocolId &&
         header.version == thisProtocol.version &&
         header.flags == thisProtocol.flags;
}

bool DatagramCodec::speaksAnotherVersion(const PacketHeader& header) {
  const PacketHeader thisProtocol;
  return header.protocolId == thisProtocol.protocolId &&
         header.version != thisProtocol.version;
}

std::vector<MessageFrame> DatagramCodec::splitMessages(
    std::span<const std::byte> datagram) {
  std::vector<MessageFrame> messages;
  if (!forEachMessage(datagram, [&messages](const MessageFrame& message) {
        messages.push_back(message);
      })) {
    throw MalformedDatagramException(datagram.size());
  }
  return messages;
}

std::optional<DatagramCodec::MessageTypes> DatagramCodec::messageTypes(
    std::span<const std::byte> datagram) {
  MessageTypes types;
  if (!forEachMessage(datagram, [&types](const MessageFrame& message) {
        types.set(message.type);
      })) {
    return std::nullopt;
  }
  return types;
}

}  // namespace rtype::network
