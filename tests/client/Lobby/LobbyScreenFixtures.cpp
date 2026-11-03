#include "LobbyScreenFixtures.hpp"
#include <cstdint>
#include "LobbyState.hpp"

rtype::network::LobbyPlayer pilot(std::uint8_t number, const char* nickname,
                                  bool ready) {
  return {.playerNumber = number, .ready = ready, .nickname = nickname};
}
