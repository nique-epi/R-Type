#include "BossStatus.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EntityIdConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

bool isListed(BossStage stage) {
  switch (stage) {
    case BossStage::Arriving:
    case BossStage::Fighting:
    case BossStage::Enraged:
    case BossStage::Defeated:
      return true;
  }
  return false;
}

/** @brief Refuses an Arriving status that names an entity or a health. */
void validateArrival(const BossStatus& message) {
  if (message.bossEntityId != identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(BOSS_ENTITY_ID_FIELD,
                                     message.bossEntityId);
  }
  if (message.fullHealth != 0) {
    throw InvalidFieldValueException(FULL_HEALTH_FIELD, message.fullHealth);
  }
  if (message.health != 0) {
    throw InvalidFieldValueException(HEALTH_FIELD, message.health);
  }
}

/** @brief Refuses a status of a boss in play without an entity, without a
 * full health, with more health than full, or whose health of 0 does not go
 * with Defeated. */
void validateFight(const BossStatus& message) {
  if (message.bossEntityId == identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(BOSS_ENTITY_ID_FIELD,
                                     message.bossEntityId);
  }
  if (message.fullHealth == 0) {
    throw InvalidFieldValueException(FULL_HEALTH_FIELD, message.fullHealth);
  }
  if (message.health > message.fullHealth ||
      (message.health == 0) != (message.stage == BossStage::Defeated)) {
    throw InvalidFieldValueException(HEALTH_FIELD, message.health);
  }
}

void validate(const BossStatus& message) {
  if (!isListed(message.stage)) {
    throw InvalidFieldValueException(STAGE_FIELD,
                                     static_cast<std::uint8_t>(message.stage));
  }
  if (message.stage == BossStage::Arriving) {
    validateArrival(message);
    return;
  }
  validateFight(message);
}

}  // namespace

void encodePayload(ByteWriter& writer, const BossStatus& message) {
  validate(message);
  writer.writeUint32(message.bossEntityId);
  writer.writeUint16(message.health);
  writer.writeUint16(message.fullHealth);
  writer.writeUint8(static_cast<std::uint8_t>(message.stage));
}

BossStatus decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<BossStatus> type) {
  ByteReader reader{payload};
  const BossStatus message{.bossEntityId = reader.readUint32(),
                           .health = reader.readUint16(),
                           .fullHealth = reader.readUint16(),
                           .stage = static_cast<BossStage>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
