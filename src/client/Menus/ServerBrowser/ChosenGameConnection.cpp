#include "ChosenGameConnection.hpp"
#include "ChosenGame.hpp"
#include "IScreenNavigator.hpp"
#include "MenuScreenIdConstants.hpp"
#include "ServerChoice.hpp"

namespace rtype::client {

void openConnectionTo(ServerChoice& choice, const ChosenGame& game,
                      IScreenNavigator& navigator) {
  choice.lastServer = game.server;
  choice.lastRequest = game.request;
  choice.nextServers = game.otherServers;
  navigator.push(CONNECTION_TO_MAP_SERVER_SCREEN);
}

}  // namespace rtype::client
