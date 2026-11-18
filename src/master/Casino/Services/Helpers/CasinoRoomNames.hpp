#pragma once

#include <cstdint>
#include <string>

namespace rtype::master {

/** @returns The name of the room @p room of the casino: one of
 * CASINO_ROOM_NAMES while the list lasts, `Room <room>` past it. */
[[nodiscard]] std::string casinoRoomName(std::uint8_t room);

}  // namespace rtype::master
