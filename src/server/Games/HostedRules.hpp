#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "GameRuleConstants.hpp"

namespace rtype::server {

/** @brief The rules a game is hosted with: the files it reads, the seed it
 * starts from, the delay it holds each input, the lives each player starts
 * with and whether friendly fire is on. */
struct HostedRules {
  /** @brief WaveCatalog::fingerprint() of the waves file. */
  std::uint64_t wavesFingerprint{0};
  /** @brief entitySheetsFingerprint() of the entity sheets. */
  std::uint64_t sheetsFingerprint{0};
  /** @brief The seed the game starts from; empty when it draws its own. */
  std::optional<std::uint32_t> seed;
  /** @brief The ticks the game holds each input. */
  std::size_t inputDelayTicks{0};
  /** @brief The lives each player starts the game with. */
  std::uint8_t lives{identifiers::DEFAULT_LIVES};
  /** @brief Whether a missile of a player costs another player a life. */
  bool friendlyFire{false};
};

}  // namespace rtype::server
