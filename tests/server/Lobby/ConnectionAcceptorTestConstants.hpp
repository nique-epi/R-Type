#pragma once

#include <string_view>

namespace rtype::server::testing {

/**
 * @brief The start of the nickname nicknameOf() gives each player, followed by
 * the number of the player: Pilot0, Pilot1…
 */
constexpr std::string_view NICKNAME_PREFIX = "Pilot";

}  // namespace rtype::server::testing
