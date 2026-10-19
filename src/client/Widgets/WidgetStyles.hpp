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

/**
 * @brief Description of the selected bar of a form, in small print, then its
 * status line: both inside its panel, so not outlined.
 */
constexpr TextStyle FORM_DESCRIPTION_STYLE{.font = TextFont::Small,
                                           .color = FORM_DESCRIPTION_COLOR};
constexpr TextStyle STATUS_STYLE{.font = TextFont::Regular,
                                 .color = STATUS_TEXT_COLOR};

/** @brief Title and question of a dialog, inside its panel, so not
 * outlined. */
constexpr TextStyle DIALOG_HEADING_STYLE{.font = TextFont::Regular,
                                         .color = DIALOG_HEADING_COLOR};
constexpr TextStyle DIALOG_MESSAGE_STYLE{.font = TextFont::Regular,
                                         .color = DIALOG_MESSAGE_COLOR};

/** @brief A notice in the top-right corner of a tabbed panel, such as the game
 * going on behind the options: small print in gold, outlined. */
constexpr TextStyle NOTICE_STYLE{.font = TextFont::Small,
                                 .color = STATUS_TEXT_COLOR,
                                 .outlined = true,
                                 .alignment = TextAlignment::Right};

/** @brief A word stamped at the end of the rule of a tabbed panel, such as
 * SAVED: small print in gold, outlined, on the sky. */
constexpr TextStyle STAMP_STYLE{
    .font = TextFont::Small, .color = STATUS_TEXT_COLOR, .outlined = true};

/** @brief The status line of a room, under its bars: gold, outlined on the
 * sky, centered. */
constexpr TextStyle ROOM_STATUS_STYLE{.font = TextFont::Regular,
                                      .color = STATUS_TEXT_COLOR,
                                      .outlined = true,
                                      .alignment = TextAlignment::Center};

}  // namespace rtype::client
