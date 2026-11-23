#pragma once

#include <cstdint>
#include <string>
#include "FinishedGame.hpp"

/** @returns A vanilla game of two where ACE scored @p score, joined with
 * @p ticket. */
[[nodiscard]] rtype::network::FinishedGame gameOfAceWith(
    const std::string& ticket, std::uint32_t score);
