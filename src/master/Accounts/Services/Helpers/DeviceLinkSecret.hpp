#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns A new secret of a device link: network::DEVICE_LINK_SECRET_PREFIX,
 * then network::DEVICE_LINK_SECRET_BYTES drawn by
 * network::fillWithRandomBytes() in lowercase hexadecimal, as the tokens of the
 * sessions are drawn.
 */
[[nodiscard]] std::string newDeviceLinkSecret();

/**
 * @returns The fingerprint of @p secret kept in the database in place of the
 * secret: its hash by network::hashOf() in DEVICE_LINK_FINGERPRINT_CONTEXT, in
 * hexadecimal. A secret is 256 random bits, so a fast hash is enough.
 */
[[nodiscard]] std::string deviceLinkFingerprintOf(std::string_view secret);

/**
 * @returns A new short code: network::DEVICE_LINK_USER_CODE_LETTERS letters of
 * network::DEVICE_LINK_USER_CODE_ALPHABET, drawn without bias (a random byte
 * from USER_CODE_UNBIASED_LIMIT up is drawn again), in two groups joined by
 * network::DEVICE_LINK_USER_CODE_SEPARATOR: `KQ7F-4ZPB` has the shape.
 */
[[nodiscard]] std::string newDeviceLinkUserCode();

}  // namespace rtype::master
