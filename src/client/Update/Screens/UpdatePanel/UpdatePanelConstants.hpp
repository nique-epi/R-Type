#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "InterfaceColorConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

/** @brief Texts of the update panel. */
constexpr std::string_view UPDATE_HEADING = "UPDATE AVAILABLE";
constexpr std::string_view VERSION_PREFIX = "VER ";
constexpr std::string_view VERSION_ARROW = " > VER ";
constexpr std::string_view DOWNLOADING_TEXT = "DOWNLOADING ";
constexpr std::string_view PERCENT_SIGN = "%";
constexpr std::string_view INSTALLING_TEXT = "INSTALLING";
constexpr std::string_view RESTARTING_TEXT = "RESTARTING";
constexpr std::string_view UPDATE_FAILED_TEXT = "UPDATE FAILED";

/** @brief The bar of the panel, from its top-left corner: as wide as the
 * panel less its margins of 16 pixels, a border of PANEL_BORDER_COLOR around a
 * fill of HUD_CYAN; then the status line under it. */
constexpr sf::Vector2i UPDATE_BAR_OFFSET(16, 52);
constexpr sf::Vector2i UPDATE_BAR_SIZE(240, 8);
constexpr int UPDATE_BAR_BORDER = 1;
constexpr sf::Color UPDATE_BAR_BORDER_COLOR = PANEL_BORDER_COLOR;
constexpr sf::Color UPDATE_BAR_FILL_COLOR = HUD_CYAN;
constexpr sf::Vector2i UPDATE_STATUS_OFFSET(16, 72);

/** @brief How long RESTARTING shows before the client closes, and UPDATE
 * FAILED before the panel lets the player go. */
constexpr engine::Duration RESTART_MESSAGE_DURATION =
    std::chrono::milliseconds(1000);
constexpr engine::Duration FAILED_MESSAGE_DURATION =
    std::chrono::milliseconds(3000);

}  // namespace rtype::client
