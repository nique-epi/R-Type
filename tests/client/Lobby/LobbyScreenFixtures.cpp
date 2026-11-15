#include "LobbyScreenFixtures.hpp"
#include <cstdint>
#include "LobbyState.hpp"

rtype::network::LobbyPlayer pilot(std::uint8_t number, const char* nickname,
                                  bool ready) {
  return {.playerNumber = number,
          .place = ready ? rtype::network::PilotPlace::Ready
                         : rtype::network::PilotPlace::NotReady,
          .nickname = nickname};
}

rtype::network::LobbyPlayer pilotAtTheCasino(std::uint8_t number,
                                             const char* nickname) {
  return {.playerNumber = number,
          .place = rtype::network::PilotPlace::AtCasino,
          .nickname = nickname};
}
