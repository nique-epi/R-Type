#pragma once

#include <string_view>

namespace rtype::network {

/**
 * @brief The `detail` of the answers of the master that refuse bets for a
 * reason a player can act on: the routes answer two of them with 409 and the
 * rules of the cloth and the wallet with 422, so a client tells them apart by
 * this text.
 */
constexpr std::string_view BETS_CLOSED_DETAIL =
    "the bets of that round are closed";
constexpr std::string_view NOT_SEATED_DETAIL =
    "take a seat in the room before betting";
constexpr std::string_view NOT_ENOUGH_CHIPS_DETAIL =
    "the bets hold more chips than the wallet";

}  // namespace rtype::network
