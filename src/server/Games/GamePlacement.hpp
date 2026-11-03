#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include "GameVacancy.hpp"

namespace rtype::server {

/**
 * @returns The index of the first game of @p games that waits for players and
 * is not full, or nothing when none is. What a new player gets when none is
 * left is the caller's choice.
 */
[[nodiscard]] std::optional<std::size_t> firstJoinableGame(
    std::span<const GameVacancy> games);

}  // namespace rtype::server
