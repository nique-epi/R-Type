#pragma once

#include <span>
#include <string>
#include <vector>
#include "Language.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @returns The texts of @p labels in @p language, in their order: the
 * choices of a selector or the tabs of a strip. */
[[nodiscard]] std::vector<std::string> textsIn(
    std::span<const Translation> labels, Language language);

}  // namespace rtype::client
