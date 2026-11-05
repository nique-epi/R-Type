#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rtype::server {

/** @brief The rules a server hosts its games with: the files they read, the
 * seed they start from and the delay they hold each input. */
struct HostedRules {
  /** @brief WaveCatalog::fingerprint() of the waves file. */
  std::uint64_t wavesFingerprint{0};
  /** @brief entitySheetsFingerprint() of the entity sheets. */
  std::uint64_t sheetsFingerprint{0};
  /** @brief The seed the games start from; empty when each draws its own. */
  std::optional<std::uint32_t> seed;
  /** @brief The ticks every game holds each input. */
  std::size_t inputDelayTicks{0};
};

}  // namespace rtype::server
