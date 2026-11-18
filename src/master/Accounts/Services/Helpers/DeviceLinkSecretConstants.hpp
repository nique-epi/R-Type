#pragma once

#include <cstddef>
#include <string_view>
#include "MasterMessageConstants.hpp"

namespace rtype::master {

/**
 * @brief The context of hashOf() the fingerprint of the secret of a device
 * link is computed in: exactly network::HASH_CONTEXT_LENGTH characters, unlike
 * the contexts of the tokens, the keys and the tickets, so a secret never
 * shares a fingerprint with them.
 */
constexpr std::string_view DEVICE_LINK_FINGERPRINT_CONTEXT = "devlink1";

/**
 * @brief The values a random byte takes, and the first byte drawn again when
 * a letter of a short code is drawn: 240, the largest multiple of the 20
 * letters of network::DEVICE_LINK_USER_CODE_ALPHABET up to 256, so each letter
 * comes from 12 byte values, none from more.
 */
constexpr std::size_t BYTE_VALUES = 256;
constexpr std::size_t USER_CODE_UNBIASED_LIMIT =
    BYTE_VALUES -
    (BYTE_VALUES % network::DEVICE_LINK_USER_CODE_ALPHABET.size());

}  // namespace rtype::master
