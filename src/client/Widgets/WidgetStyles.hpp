#pragma once

#include "TextStyle.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

/**
 * @brief Lines of a page, laid on the sky and outlined: its heading, its
 * subtitle and the description of the selected button.
 */
constexpr TextStyle PAGE_LINE_STYLE{
    .font = TextFont::Regular, .color = PAGE_TEXT_COLOR, .outlined = true};

/** @brief Small print of a page: the version at the top of the column, the
 * footer in the bottom-right corner. */
constexpr TextStyle VERSION_STYLE{
    .font = TextFont::Small, .color = PAGE_TEXT_COLOR, .outlined = true};
constexpr TextStyle FOOTER_STYLE{.font = TextFont::Small,
                                 .color = PAGE_TEXT_COLOR,
                                 .outlined = true,
                                 .alignment = TextAlignment::Right};

/** @brief Title and question of a dialog, inside its panel, so not
 * outlined. */
constexpr TextStyle DIALOG_HEADING_STYLE{.font = TextFont::Regular,
                                         .color = DIALOG_HEADING_COLOR};
constexpr TextStyle DIALOG_MESSAGE_STYLE{.font = TextFont::Regular,
                                         .color = DIALOG_MESSAGE_COLOR};

}  // namespace rtype::client
