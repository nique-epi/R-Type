#pragma once

#include <string>
#include "ContentFingerprint.hpp"

namespace rtype::network {

/** @returns @p fingerprint in lowercase hexadecimal, two digits per byte, as
 * the path of the route of a pack and the name of a pack in a cache write it.
 */
[[nodiscard]] std::string fingerprintText(
    const ContentFingerprint& fingerprint);

}  // namespace rtype::network
