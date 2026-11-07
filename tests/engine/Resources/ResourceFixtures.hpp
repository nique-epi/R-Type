#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

/** @returns @p views as texts, in their order: the lines and names the
 * functions under test read. */
[[nodiscard]] std::vector<std::string> textsOf(
    std::span<const std::string_view> views);
