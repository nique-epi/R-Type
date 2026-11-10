#pragma once

#include <span>
#include "FeedConstants.hpp"
#include "SignedFeed.hpp"

namespace rtype::client {

/** @returns Whether the signature of @p feed, ED25519_SIGNATURE_BYTES long, is
 * the Ed25519 signature of its text by the key whose public half is
 * @p publicKey, as `openssl pkeyutl -sign -rawin` writes it. */
[[nodiscard]] bool isSignedBy(
    const SignedFeed& feed,
    std::span<const unsigned char, ED25519_PUBLIC_KEY_BYTES> publicKey);

}  // namespace rtype::client
