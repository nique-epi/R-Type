#pragma once

#include <cstddef>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "GamesFixture.hpp"

/** @brief The tests of the games as the master lists them: games of the
 * server, and players seated in them under nicknames of their own. */
class HostedGamesFixture : public GamesFixture {
 protected:
  /** @brief Seats the player at @p index in @p game, named @p nickname. */
  static rtype::server::ConnectedPlayer& seat(rtype::server::GameInstance& game,
                                              std::size_t index,
                                              const std::string& nickname);
};
