#pragma once

#include <optional>
#include "ConnectionRequest.hpp"
#include "GameRules.hpp"
#include "GameTicket.hpp"

namespace rtype::client {

/**
 * @brief The game an attempt asks for, and its rules when the client knows
 * them: those the player chose for a NewGame; the default rules for AnyGame,
 * since a server only places such a player in a game played by them; those
 * the master lists for a game chosen in the panel of the games; nothing for a
 * game named at launch. It also holds the game ticket the master gave the
 * player for this attempt, which the request carries to the game; it is kept
 * in memory only, never logged and never written.
 */
struct GameRequest {
  network::GameChoice game{network::ANY_GAME_CHOICE};
  std::optional<network::GameRules> rules = std::nullopt;
  /** @brief The ticket of the attempt; network::GUEST_TICKET while the
   * player plays as a guest, or when the master gave no ticket in time. */
  network::GameTicket ticket{network::GUEST_TICKET};

  bool operator==(const GameRequest&) const = default;
};

/** @returns A request for @p game with the rules the client knows of it: the
 * default rules for AnyGame and NewGame, nothing for ExistingGame. */
[[nodiscard]] GameRequest gameRequestFor(const network::GameChoice& game);

/** @returns The rules a ConnectionRequest carries for @p request: its rules,
 * or the default ones, for a NewGame; nothing for any other game, as the
 * protocol requires. */
[[nodiscard]] std::optional<network::GameRules> rulesSentFor(
    const GameRequest& request);

}  // namespace rtype::client
