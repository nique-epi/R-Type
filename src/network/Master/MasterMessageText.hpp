#pragma once

#include <string>
#include <string_view>
#include "ServerHeartbeat.hpp"
#include "ServerRegistration.hpp"

namespace rtype::network {

/**
 * @returns Whether @p name is a name a game server may have: 1 to
 * MAXIMUM_SERVER_NAME_SIZE bytes of printable UTF-8 (isPrintableUtf8()).
 */
[[nodiscard]] bool isValidServerName(std::string_view name);

/**
 * @returns @p registration as the JSON body of `PUT /api/servers/me`, its
 * address left out when it is empty. A text that is not UTF-8 is written with
 * U+FFFD in place of its bad bytes, so writing never throws.
 */
[[nodiscard]] std::string registrationText(
    const ServerRegistration& registration);

/**
 * @returns The registration a server sent as @p text. A member it does not
 * know is ignored, so a newer server still registers.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, of another type or out of its bounds: a name or a version that is
 * empty, too long or not printable UTF-8, an address that is no IPv4 address,
 * a port outside LOWEST_PORT to HIGHEST_PORT, a number of games outside
 * MINIMUM_HOSTED_GAMES to MAXIMUM_HOSTED_GAMES or of players outside
 * MINIMUM_PLAYERS_PER_GAME to MAXIMUM_PLAYERS.
 */
[[nodiscard]] ServerRegistration registrationFromText(std::string_view text);

/**
 * @returns @p heartbeat as the JSON body of `PUT /api/servers/me/heartbeat`,
 * every empty measure left out, written as registrationText() writes.
 */
[[nodiscard]] std::string heartbeatText(const ServerHeartbeat& heartbeat);

/**
 * @returns The heartbeat a server sent as @p text; a member it does not know
 * is ignored.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, of another type or out of its bounds: more than
 * MAXIMUM_HOSTED_GAMES games, two games of one number, a number below
 * FIRST_HOSTED_GAME_NUMBER, a state that is not one of
 * HOSTED_GAME_STATE_TEXTS, a difficulty or a player that is no valid nickname,
 * more than MAXIMUM_PLAYERS players in a game, a measure that is negative or
 * not a number, a loss above MAXIMUM_LOSS_PERCENT.
 */
[[nodiscard]] ServerHeartbeat heartbeatFromText(std::string_view text);

}  // namespace rtype::network
