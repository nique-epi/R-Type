#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <string_view>
#include "FlashPreference.hpp"
#include "InterfaceColorConstants.hpp"
#include "PlayerPalette.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns What paintIntroCast() asks a canvas at @p real seconds of the
 * music, the cast built from INTRO_TEST_SEED, without shake, the freezes
 * played unless @p preference reduces flashes, the pilots painted in
 * @p palette. */
[[nodiscard]] InterfaceCanvasRecord paintedAt(
    float real, rtype::client::FlashPreference preference,
    const rtype::client::PlayerPalette& palette = rtype::client::PLAYER_COLORS);

/** @returns How many images of a sheet the record holds whose rectangle
 * starts on row @p top. */
[[nodiscard]] std::size_t imagesOnRow(const InterfaceCanvasRecord& record,
                                      std::string_view assetId, int top);

/** @returns How many images the record holds tinted @p tint. */
[[nodiscard]] std::size_t imagesTinted(const InterfaceCanvasRecord& record,
                                       sf::Color tint);

/** @returns How many fills of the record are painted in @p color. */
[[nodiscard]] std::size_t fillsColored(const InterfaceCanvasRecord& record,
                                       sf::Color color);

/** @returns How many fills of the record are speed lines: one pixel high, at
 * least SHORTEST_SPEED_LINE long. */
[[nodiscard]] std::size_t speedLinesOf(const InterfaceCanvasRecord& record);
