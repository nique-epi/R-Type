#pragma once

#include "ActionKey.hpp"
#include "DataFileTable.hpp"

namespace rtype::game {

/** @returns The attack and the phase @p table names, `attack` and `phase`,
 * both required.
 * @throws What engine::wholeNumberAt() and engine::choiceAt() throw. */
[[nodiscard]] ActionKey actionKeyAt(const engine::DataFileTable& table);

}  // namespace rtype::game
