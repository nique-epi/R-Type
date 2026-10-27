#pragma once

#include <string>

/** @returns A line of a taunt, its marks of names replaced by a nickname of
 * 16 characters. */
[[nodiscard]] std::string withLongestName(std::string line);
