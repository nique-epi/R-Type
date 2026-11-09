#pragma once

#include <optional>
#include <string_view>
#include "ReleaseOffer.hpp"

namespace rtype::client {

/**
 * @returns What @p manifest offers @p system (Linux, Darwin or win64), or
 * nothing when it is no JSON object, or its version, its notes or the package
 * of that system is missing or out of bounds: a package file name of letters,
 * digits, dots, dashes and underscores not starting with a dot, a size of 1 to
 * MAXIMUM_PACKAGE_BYTES, a SHA-256 of SHA256_HEXADECIMAL_SIZE lowercase
 * hexadecimal digits, at most MAXIMUM_NOTE_LINES notes of printable ASCII.
 * The signature is not checked here: ReleaseCheck checks it first.
 */
[[nodiscard]] std::optional<ReleaseOffer> releaseOfferOf(
    std::string_view manifest, std::string_view system);

}  // namespace rtype::client
