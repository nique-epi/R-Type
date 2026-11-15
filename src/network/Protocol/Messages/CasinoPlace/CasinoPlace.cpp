#include "CasinoPlace.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/**
 * @throws InvalidFieldValueException when @p place is not one of TablePlace.
 */
void requireListedPlace(TablePlace place) {
  switch (place) {
    case TablePlace::InRoom:
    case TablePlace::AtTable:
      return;
  }
  throw InvalidFieldValueException(PLACE_FIELD,
                                   static_cast<std::uint8_t>(place));
}

}  // namespace

void encodePayload(ByteWriter& writer, const CasinoPlace& message) {
  requireListedPlace(message.place);
  writer.writeUint8(static_cast<std::uint8_t>(message.place));
}

CasinoPlace decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<CasinoPlace> type) {
  ByteReader reader{payload};
  const CasinoPlace message{.place =
                                static_cast<TablePlace>(reader.readUint8())};
  requireListedPlace(message.place);
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network
