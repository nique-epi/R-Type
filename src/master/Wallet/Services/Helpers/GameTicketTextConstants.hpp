#pragma once

#include <string_view>

namespace rtype::master {

/**
 * @brief The context of hashOf() the fingerprint of a game ticket is computed
 * in: exactly network::HASH_CONTEXT_LENGTH characters, unlike the contexts of
 * the tokens and the keys, so a ticket never shares a fingerprint with them.
 */
constexpr std::string_view GAME_TICKET_FINGERPRINT_CONTEXT = "gametkt1";

}  // namespace rtype::master
