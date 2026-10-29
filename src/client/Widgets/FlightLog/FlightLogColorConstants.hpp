#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstdint>
#include "BackgroundColorConstants.hpp"
#include "EffectColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "SpriteToneColorConstants.hpp"

namespace rtype::client {

/** @brief The ink of the paper pages and of the taunt: text, fainter dots
 * and zeros, small print, and the ember stamp. */
constexpr sf::Color PAPER_COLOR = FLASH_COLOR;
constexpr sf::Color PAPER_INK = INK;
constexpr sf::Color PAPER_LABEL_COLOR = BLUE;
constexpr sf::Color PAPER_FAINT_COLOR = PALE_BLUE;
constexpr sf::Color STAMP_COLOR = EMBER;

/** @brief The sides of the closed book: the back cover at the bottom of the
 * stack, the edges of the pages and the paper between them; also the spine
 * of the open book. */
constexpr sf::Color BOOK_EDGE_COLOR = INK;
constexpr sf::Color PAGE_EDGE_COLOR = PLANET_RING_FRONT_COLOR;
constexpr sf::Color PAGE_BLOCK_COLOR = FLASH_COLOR;

/** @brief Tape: star white at 55 %, so the paper shows through. */
constexpr std::uint8_t TAPE_ALPHA = 140;
constexpr sf::Color TAPE_COLOR(STAR_WHITE.r, STAR_WHITE.g, STAR_WHITE.b,
                               TAPE_ALPHA);

/** @brief The polaroids: their paper, the sky in their window, the stars
 * in it; and YOU on ink in a row of the ranking. */
constexpr sf::Color POLAROID_COLOR = STAR_WHITE;
constexpr sf::Color POLAROID_WINDOW_COLOR = SKY_COLOR;
constexpr std::array POLAROID_STAR_COLORS{PALE_BLUE, STAR_WHITE, BLUE};
constexpr sf::Color YOU_TAG_COLOR = INK;
constexpr sf::Color YOU_TAG_TEXT_COLOR = STAR_WHITE;

constexpr sf::Color RIBBON_COLOR = PLAYER_PINK;
constexpr sf::Color RIBBON_SHADE_COLOR = EMBER;
constexpr sf::Color RIBBON_NOTCH_COLOR = SKY_COLOR;
constexpr std::array DUST_COLORS{PALE_BLUE, STAR_WHITE};

/** @brief PRESS ENTER TO REPLAY: gold with a dark orange glow; under the
 * pointer, yellow with a gold glow; pressed, star white. */
constexpr sf::Color PROMPT_COLOR = FIRE_GOLD;
constexpr sf::Color PROMPT_GLOW_COLOR = PLAYER_DARK_ORANGE;
constexpr sf::Color HOVERED_PROMPT_COLOR = FIRE_YELLOW;
constexpr sf::Color HOVERED_PROMPT_GLOW_COLOR = FIRE_GOLD;
constexpr sf::Color PRESSED_PROMPT_COLOR = STAR_WHITE;

/** @brief QUIT: red on the sky; under the pointer, filled red under ink;
 * pressed, ember under star white. */
constexpr sf::Color QUIT_COLOR = FIRE_RED;
constexpr sf::Color QUIT_INSIDE_COLOR = SKY_COLOR;
constexpr sf::Color HOVERED_QUIT_INK = INK;
constexpr sf::Color PRESSED_QUIT_INSIDE_COLOR = EMBER;
constexpr sf::Color PRESSED_QUIT_INK = STAR_WHITE;

}  // namespace rtype::client
