#pragma once

#include "BotStanding.hpp"

namespace rtype::tools {

/**
 * @returns Whether a bot standing as @p bot asks to be ready now: it was
 * accepted, its game has not started, it has not asked yet in this waiting
 * room, and @p anyBotConnecting is false, no bot of the squad still waiting
 * for the answer to its connection.
 *
 * The server starts a game once every player connected to it is ready, from
 * one player on, and puts a new player in the first game that waits with a
 * free seat. A bot that asked as soon as it was accepted would start its game
 * alone, and the next bot would land in the next game: every game would hold
 * one bot. Waiting until every bot has its answer fills each game first. Once
 * a game ended, its players stay connected and none is ready: each bot asks
 * again, and the game starts full again.
 */
[[nodiscard]] bool asksReady(const BotStanding& bot, bool anyBotConnecting);

}  // namespace rtype::tools
