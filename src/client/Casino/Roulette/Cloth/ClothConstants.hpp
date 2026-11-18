#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include "ClothBand.hpp"
#include "ClothPlace.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteConstants.hpp"

namespace rtype::client {

/**
 * @brief The numbers of the cloth, read across as the server reads them:
 * CLOTH_STREETS streets of NUMBERS_PER_STREET, each a column on screen, the
 * first number of a street on the bottom row.
 */
constexpr int CLOTH_STREETS =
    network::HIGHEST_ROULETTE_NUMBER / network::NUMBERS_PER_STREET;
constexpr int CLOTH_ROWS = network::NUMBERS_PER_STREET;

/**
 * @brief The places of the band of the numbers (ClothPlace): two steps across
 * per street, a column and the line after it, the zero at ZERO_ACROSS and the
 * columns at NUMBERS_LAST_ACROSS; two steps down per row, the street edge at
 * STREET_EDGE_DOWN. NUMBERS_LAST_ROW_DOWN is the row of the first number of
 * each street.
 */
constexpr int ZERO_ACROSS = 0;
constexpr int NUMBERS_LAST_ACROSS = 2 * CLOTH_STREETS;
constexpr int NUMBERS_LAST_ROW_DOWN = 2 * (CLOTH_ROWS - 1);
constexpr int STREET_EDGE_DOWN = NUMBERS_LAST_ROW_DOWN + 1;
constexpr int ROW_STEP_DOWN = 2;

/** @brief The dozens under the numbers, and the steps across one of them
 * spans in the band of the numbers. */
constexpr int CLOTH_DOZENS =
    network::HIGHEST_ROULETTE_NUMBER / network::NUMBERS_PER_DOZEN;
constexpr int ACROSS_PER_DOZEN =
    2 * (network::NUMBERS_PER_DOZEN / network::NUMBERS_PER_STREET);

/** @brief The even chances under the dozens, from the left, as the cloth
 * shows them: 1-18, EVEN, the red diamond, the black one, ODD, 19-36; two
 * under each dozen. */
constexpr std::array EVEN_CHANCE_KINDS{
    network::RouletteBetKind::Low, network::RouletteBetKind::Even,
    network::RouletteBetKind::Red, network::RouletteBetKind::Black,
    network::RouletteBetKind::Odd, network::RouletteBetKind::High};
constexpr int EVEN_CHANCES_PER_DOZEN =
    static_cast<int>(EVEN_CHANCE_KINDS.size()) / CLOTH_DOZENS;

/** @brief Where the cursor stands when the table opens: on 17, in the
 * middle of the cloth. */
constexpr ClothPlace FIRST_CLOTH_PLACE{
    .band = ClothBand::Numbers, .across = 11, .down = 2};

/**
 * @brief The cloth as roulette_table.png draws it, in pixels of the interface
 * once the table is laid: the top-left corner of the number 3, the size of a
 * number between its lines, the width of the zero on the left and of the
 * columns on the right, and the height of the dozens and of the even chances.
 */
constexpr sf::Vector2i CLOTH_GRID_TOP_LEFT(172, 100);
constexpr sf::Vector2i CLOTH_CELL_SIZE(21, 18);
constexpr int CLOTH_ZERO_WIDTH = 18;
constexpr int CLOTH_COLUMNS_WIDTH = 26;
constexpr int CLOTH_OUTSIDE_ROW_HEIGHT = 15;

/** @brief The box of the cursor over a line or a corner, around the point
 * the chips lie on. */
constexpr sf::Vector2i CLOTH_LINE_SPOT_SIZE(9, 9);

}  // namespace rtype::client
