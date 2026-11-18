#pragma once

#include <string>
#include <string_view>

/** @returns @p code as a player in a hurry types it: in lowercase, without
 * the separator of its two groups. */
[[nodiscard]] std::string lowercaseWithoutSeparator(std::string_view code);
