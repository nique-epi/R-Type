#pragma once

#include <string_view>

namespace rtype::master {

/**
 * @brief The context of hashOf() the fingerprint of a token is computed in:
 * exactly network::HASH_CONTEXT_LENGTH characters, unlike FINGERPRINT_CONTEXT
 * of the keys, so a token and a key never share a fingerprint.
 */
constexpr std::string_view SESSION_FINGERPRINT_CONTEXT = "sesstok1";

}  // namespace rtype::master
