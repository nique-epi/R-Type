#pragma once

#include "ChosenGame.hpp"
#include "IScreenNavigator.hpp"
#include "ServerChoice.hpp"

namespace rtype::client {

/**
 * @brief Puts @p game in the last attempt of @p choice, its server and its
 * request together, with the servers to try after a full one, and pushes
 * CONNECTION_TO_MAP_SERVER_SCREEN, which asks for the nickname, then connects.
 * The server map calls it over itself; the new game dialog pops itself first,
 * so BACK from the connection comes back to the map.
 */
void openConnectionTo(ServerChoice& choice, const ChosenGame& game,
                      IScreenNavigator& navigator);

}  // namespace rtype::client
