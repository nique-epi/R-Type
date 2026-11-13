#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "LineWrapping.hpp"

namespace rtype::client {

/**
 * @returns @p text cut into lines no wider than wrapping.width in the font of
 * @p wrapping, as @p canvas measures it at its text size: a line breaks at
 * the space before a word that would pass the width, and a word wider than a
 * line is cut between two letters, as many letters on each line as fit, at
 * least one.
 */
[[nodiscard]] std::vector<std::string> wrappedLinesOf(
    std::string_view text, LineWrapping wrapping,
    const IInterfaceCanvas& canvas);

}  // namespace rtype::client
