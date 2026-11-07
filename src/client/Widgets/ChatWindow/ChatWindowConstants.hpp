#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "InterfaceConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Left edge of the lines and of the field of the chat. */
constexpr int CHAT_LEFT = SAFE_MARGIN;

/** @brief Rows a line takes: its letters, and the outline above and under
 * them, so two outlines never touch. */
constexpr int CHAT_LINE_PITCH =
    TEXT_LINE_HEIGHT + (SIDES_PER_AXIS * BORDER_THICKNESS);

/** @brief The field: a line and FIELD_TEXT_PADDING above and under it; SAY,
 * its label, FIELD_LABEL_GAP above it, and the keys written at its right,
 * CHAT_HINT_GAP at least after SAY. */
constexpr int CHAT_FIELD_HEIGHT =
    TEXT_LINE_HEIGHT + (SIDES_PER_AXIS * FIELD_TEXT_PADDING);
constexpr const char* CHAT_FIELD_LABEL = "SAY";
constexpr const char* CHAT_FIELD_HINT = "ENTER SENDS  ESC CLOSES";
constexpr int CHAT_HINT_GAP = 8;

/** @brief What a line starts with, after the name of its pilot; and what
 * says how many lines lie above the open chat. */
constexpr std::string_view CHAT_NAME_SEPARATOR = ": ";
constexpr std::string_view MORE_LINES_START = "PGUP: ";
constexpr std::string_view MORE_LINES_END = " MORE LINES";

/** @brief Keys of the open chat: send, close, and show older or newer lines
 * by CHAT_SCROLLED_LINES. */
constexpr sf::Keyboard::Key CHAT_SEND_KEY = MENU_CONFIRM_KEY;
constexpr sf::Keyboard::Key CHAT_CLOSE_KEY = MENU_CANCEL_KEY;
constexpr sf::Keyboard::Key OLDER_CHAT_KEY = sf::Keyboard::Key::PageUp;
constexpr sf::Keyboard::Key NEWER_CHAT_KEY = sf::Keyboard::Key::PageDown;
constexpr int CHAT_SCROLLED_LINES = 4;

/** @brief A line fades out over the last CHAT_FADE_TIME of the time it
 * shows; the alpha of an opaque color, and how many percent make it. */
constexpr engine::Duration CHAT_FADE_TIME = std::chrono::seconds(1);
constexpr std::uint8_t OPAQUE_ALPHA = 255;
constexpr int WHOLE_PERCENT = 100;

}  // namespace rtype::client
