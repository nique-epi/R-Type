#pragma once

#include <span>
#include <string_view>
#include "ReleaseConstants.hpp"

namespace rtype::client {

/** @returns Whether @p signature, ED25519_SIGNATURE_BYTES long, is the Ed25519
 * signature of @p message by the key whose public half is @p publicKey, as
 * `openssl pkeyutl -sign -rawin` writes it. */
[[nodiscard]] bool isSignedBy(
    std::string_view message, std::string_view signature,
    std::span<const unsigned char, ED25519_PUBLIC_KEY_BYTES> publicKey);

}  // namespace rtype::client
