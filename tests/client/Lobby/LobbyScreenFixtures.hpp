#pragma once

#include <cstdint>
#include "LobbyState.hpp"

/** @returns A player the server lists in the waiting room. */
[[nodiscard]] rtype::network::LobbyPlayer pilot(std::uint8_t number,
                                                const char* nickname,
                                                bool ready);
