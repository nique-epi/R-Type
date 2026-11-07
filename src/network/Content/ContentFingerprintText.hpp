#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "ContentFingerprint.hpp"

namespace rtype::network {

/** @returns @p fingerprint in lowercase hexadecimal, two digits per byte, as
 * the path of the route of a pack and the name of a pack in a cache write it.
 */
[[nodiscard]] std::string fingerprintText(
    const ContentFingerprint& fingerprint);

/**
 * @returns The fingerprint @p text writes as fingerprintText() does, or
 * nothing when it holds anything else: another length, an uppercase digit, a
 * character that is no hexadecimal digit.
 */
[[nodiscard]] std::optional<ContentFingerprint> fingerprintFromText(
    std::string_view text);

}  // namespace rtype::network
