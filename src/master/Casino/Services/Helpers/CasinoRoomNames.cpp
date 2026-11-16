#include "CasinoRoomNames.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "CasinoHelpersConstants.hpp"
#include "CasinoRoomConstants.hpp"

namespace rtype::master {

std::string casinoRoomName(std::uint8_t room) {
  const std::size_t index = room - network::FIRST_CASINO_ROOM;
  if (room >= network::FIRST_CASINO_ROOM && index < CASINO_ROOM_NAMES.size()) {
    return std::string{CASINO_ROOM_NAMES.at(index)};
  }
  return std::string{UNNAMED_ROOM_PREFIX} + std::to_string(room);
}

}  // namespace rtype::master
