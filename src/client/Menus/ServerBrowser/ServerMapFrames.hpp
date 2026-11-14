#pragma once

#include <SFML/Window/Event.hpp>
#include "GamePanelLayout.hpp"
#include "PictureScroll.hpp"
#include "ServerMapFrame.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @returns Where the server map lays its parts at the text size of
 * @p metrics. */
[[nodiscard]] ServerMapFrame serverMapFrameOf(TextMetrics metrics);

/** @returns How far the map is scrolled after @p event: SERVER_MAP_PAGE_STEP
 * further down for Page Down, up for Page Up, never past either end; as it
 * was for any other event. */
[[nodiscard]] int pagedScrollOf(const sf::Event& event, PictureScroll scroll);

/** @returns Where the panel of the games lays its parts at the text size of
 * @p metrics: GAME_PANEL_AREA and its columns, as drawn at the size the fonts
 * are drawn at; above it, the whole map, each game on two lines, and the line
 * about the chosen game wrapped on GAME_PANEL_DESCRIPTION_LINES lines. */
[[nodiscard]] GamePanelLayout gamePanelLayoutOf(TextMetrics metrics);

}  // namespace rtype::client
