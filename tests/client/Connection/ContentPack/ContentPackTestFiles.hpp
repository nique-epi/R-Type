#pragma once

#include <cstddef>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"

namespace rtype::client::testing {

/** @returns The files of the pack of the lancer, by path. */
[[nodiscard]] std::vector<identifiers::ContentFile> lancerFiles();

/** @returns The pack of the lancer, encoded. */
[[nodiscard]] std::vector<std::byte> lancerPack();

/** @returns A pack of one file whose bytes are @p seed, so each seed gives
 * another fingerprint. */
[[nodiscard]] std::vector<std::byte> packNumbered(std::size_t seed);

/** @returns The fingerprint of @p pack. */
[[nodiscard]] network::ContentFingerprint fingerprintOf(
    const std::vector<std::byte>& pack);

}  // namespace rtype::client::testing
