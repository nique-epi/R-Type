#pragma once

#include <string>
#include "ContentFingerprint.hpp"

namespace rtype::network {

/** @returns The path a client asks the pack @p fingerprint at:
 * `/api/packs/<fingerprint in hexadecimal>`, CONTENT_PACK_ROUTE with its
 * parameter written. */
[[nodiscard]] std::string contentPackPathOf(
    const ContentFingerprint& fingerprint);

}  // namespace rtype::network
