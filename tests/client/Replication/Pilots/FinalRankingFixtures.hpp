#pragma once

#include <cstdint>
#include <initializer_list>
#include <utility>
#include <vector>
#include "GameEnded.hpp"
#include "PilotStatus.hpp"

/** @returns The board of a game of three pilots: ACE (1), NOVA (2, this
 * client), KAI (3). */
[[nodiscard]] std::vector<rtype::client::PilotStatus> boardOfThree();

/** @returns A GameEnded holding these numbers and scores, in this order. */
[[nodiscard]] rtype::network::GameEnded endedWith(
    std::initializer_list<std::pair<std::uint8_t, std::uint32_t>> scores);
