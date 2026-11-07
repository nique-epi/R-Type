#pragma once

#include <array>
#include <cstddef>
#include "ContentPackConstants.hpp"

namespace rtype::network {

/**
 * @brief The fingerprint of a content pack: the hash of every byte of the
 * encoded pack, so two packs share a fingerprint only when they hold the same
 * files, byte for byte.
 */
struct ContentFingerprint {
  std::array<std::byte, CONTENT_FINGERPRINT_BYTES> bytes{};

  bool operator==(const ContentFingerprint&) const = default;
};

}  // namespace rtype::network
