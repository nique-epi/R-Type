#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Where the fight with the boss of a game stands. The values are the
 * ones of the protocol page; any other byte is invalid.
 */
enum class BossStage : std::uint8_t {
  Arriving = 1,
  Fighting = 2,
  Enraged = 3,
  Defeated = 4,
};

/**
 * @brief The boss of the game and its health. Guaranteed and ordered: the
 * server sends one when the boss stage starts, when the fight starts, at each
 * hit, at its rage and at its death. Each holds the whole state and replaces
 * the one handed over before it, the ordered channel handing them over in the
 * order sent.
 *
 * While Arriving, bossEntityId, health and fullHealth are 0. In any other
 * stage, bossEntityId is not NO_ENTITY_ID, fullHealth is not 0, health is at
 * most fullHealth, and health is 0 exactly when Defeated.
 */
struct BossStatus {
  static constexpr MessageType TYPE = MessageType::BossStatus;

  std::uint32_t bossEntityId{};
  std::uint16_t health{};
  std::uint16_t fullHealth{};
  BossStage stage{BossStage::Arriving};

  bool operator==(const BossStatus&) const = default;
};

/**
 * @brief Writes the boss entity id, the health, the full health, then the
 * stage.
 * @throws InvalidFieldValueException when the status breaks a rule of
 * BossStatus; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const BossStatus& message);

/**
 * @brief Reads a BossStatus from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload holds fewer than 9 bytes.
 * @throws TrailingBytesException when bytes follow the stage.
 * @throws InvalidFieldValueException when the status breaks a rule of
 * BossStatus.
 */
[[nodiscard]] BossStatus decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<BossStatus> type);

}  // namespace rtype::network
