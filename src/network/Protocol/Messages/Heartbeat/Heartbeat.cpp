#include "Heartbeat.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void writeEcho(ByteWriter& writer, const HeartbeatEcho& echo) {
  writer.writeUint32(echo.sentAt);
  writer.writeUint32(echo.heldFor);
}

HeartbeatEcho readEcho(ByteReader& reader) {
  return HeartbeatEcho{.sentAt = reader.readUint32(),
                       .heldFor = reader.readUint32()};
}

/**
 * @brief Checks that the echo of a Heartbeat that carries none is 0.
 * @throws InvalidFieldValueException naming the first field that is not.
 */
void requireNoEcho(const HeartbeatEcho& echo) {
  if (echo.sentAt != 0) {
    throw InvalidFieldValueException(ECHO_SENT_AT_FIELD, echo.sentAt);
  }
  if (echo.heldFor != 0) {
    throw InvalidFieldValueException(ECHO_HELD_FOR_FIELD, echo.heldFor);
  }
}

/**
 * @brief Checks that the tick of a Heartbeat that carries none is 0.
 * @throws InvalidFieldValueException naming TICK_FIELD when it is not.
 */
void requireNoTick(std::uint32_t tick) {
  if (tick != 0) {
    throw InvalidFieldValueException(TICK_FIELD, tick);
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const Heartbeat& message) {
  writer.writeUint32(message.sentAt);
  writeBool(writer, message.echo.has_value());
  writeEcho(writer, message.echo.value_or(HeartbeatEcho{}));
  writeBool(writer, message.tick.has_value());
  writer.writeUint32(message.tick.value_or(0));
}

Heartbeat decodePayload(std::span<const std::byte> payload,
                        [[maybe_unused]] std::type_identity<Heartbeat> type) {
  ByteReader reader{payload};
  Heartbeat message{.sentAt = reader.readUint32(), .echo = std::nullopt};
  const bool echoes = readBool(reader);
  const HeartbeatEcho echo = readEcho(reader);
  const bool ticks = readBool(reader);
  const std::uint32_t tick = reader.readUint32();
  requireFullyRead(reader);
  if (echoes) {
    message.echo = echo;
  } else {
    requireNoEcho(echo);
  }
  if (ticks) {
    message.tick = tick;
  } else {
    requireNoTick(tick);
  }
  return message;
}

}  // namespace rtype::network
