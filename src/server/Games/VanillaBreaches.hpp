#pragma once

#include <string_view>
#include <vector>
#include "HostedRules.hpp"

namespace rtype::server {

/**
 * @brief Why the games a server hosts with @p rules are not vanilla, that is
 * played by the rules the game ships with, so that their scores compare with
 * the scores of other servers.
 *
 * A game is vanilla when it reads the shipped waves file and the shipped
 * entity sheets, starts from no set seed (a seed lets the enemies of every game
 * be known in advance) and holds the inputs for the default delay. A game that
 * plays a level is judged without its entity sheets: the sheets and the
 * scripts of its resources are trusted, and only a start past the first stage
 * of the level makes it leave vanilla. The server judges its own games: this
 * keeps honest changes apart, not a host who lies, and a server rebuilt with
 * other constants cannot be told apart.
 *
 * @returns MODIFIED_WAVES_BREACH, MODIFIED_ENTITY_SHEETS_BREACH,
 * IMPOSED_SEED_BREACH, CHANGED_INPUT_DELAY_BREACH, CHANGED_LIVES_BREACH,
 * FRIENDLY_FIRE_BREACH, STARTS_AT_BOSS_BREACH and
 * STARTS_PAST_THE_FIRST_STAGE_BREACH, in this order, for each rule broken;
 * empty when the games are vanilla.
 */
[[nodiscard]] std::vector<std::string_view> vanillaBreaches(
    const HostedRules& rules);

}  // namespace rtype::server
