#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @returns @p text cut into lines no wider than @p width in the regular font
 * of @p canvas, at its text size, each word whole: a line breaks at the space
 * before a word that would pass the width, and a word wider than a line has a
 * line of its own.
 */
[[nodiscard]] std::vector<std::string> wrappedLinesOf(
    std::string_view text, int width, const IInterfaceCanvas& canvas);

}  // namespace rtype::client
