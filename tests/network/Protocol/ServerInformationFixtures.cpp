#include "ServerInformationFixtures.hpp"
#include <string>
#include "ServerInformation.hpp"
#include "ServerInformationRequestTestConstants.hpp"
#include "ServerInformationTestConstants.hpp"

namespace rtype::network::testing {

ServerInformation someServerInformation() {
  return ServerInformation{.sentAt = SOME_SENT_AT,
                           .heldFor = SOME_HELD_FOR,
                           .protocolVersion = SOME_PROTOCOL_VERSION,
                           .gameVersion = std::string{SOME_GAME_VERSION},
                           .name = std::string{SOME_SERVER_NAME},
                           .playerCount = FIVE_PLAYERS_SEATED,
                           .gameCount = TWO_GAMES,
                           .freeSeatCount = THREE_FREE_SEATS};
}

}  // namespace rtype::network::testing
