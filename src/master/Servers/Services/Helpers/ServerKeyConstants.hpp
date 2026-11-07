#pragma once

#include <string_view>

namespace rtype::master {

/**
 * @brief The context of hashOf() the fingerprint of a key is computed in:
 * exactly network::HASH_CONTEXT_LENGTH characters, so no other hash of the
 * master can give the same value.
 */
constexpr std::string_view FINGERPRINT_CONTEXT = "srvkeys1";

}  // namespace rtype::master
