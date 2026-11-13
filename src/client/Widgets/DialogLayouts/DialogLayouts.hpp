#pragma once

#include "DialogLayout.hpp"
#include "DialogText.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/**
 * @returns The layout of a dialog writing @p text at @p metrics: as dialogs
 * were drawn at the size the fonts are drawn at, DIALOG_SIZE wide and its
 * message on one line; above it, as wide as its widest line needs, from
 * DIALOG_SIZE wide to DIALOG_LARGEST_WIDTH, its message wrapped inside, and
 * every line, row and button grown with the text.
 */
[[nodiscard]] DialogLayout dialogLayoutOf(const DialogText& text,
                                          TextMetrics metrics);

}  // namespace rtype::client
