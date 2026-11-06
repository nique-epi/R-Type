#pragma once

#include <string>
#include <vector>
#include "HandOverRule.hpp"

/** @returns A rule that hands over every line whose command is one of
 * @p commands. */
[[nodiscard]] rtype::engine::HandOverRule handingOver(
    std::vector<std::string> commands);
