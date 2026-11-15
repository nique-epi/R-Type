#pragma once

#include <cstdint>
#include "LobbyState.hpp"

/** @returns A player the server lists in the waiting room. */
[[nodiscard]] rtype::network::LobbyPlayer pilot(std::uint8_t number,
                                                const char* nickname,
                                                bool ready);

/** @returns A player the server lists at the table of the casino: never
 * ready. */
[[nodiscard]] rtype::network::LobbyPlayer pilotAtTheCasino(
    std::uint8_t number, const char* nickname);
