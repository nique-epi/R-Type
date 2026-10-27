#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace rtype::client::testing {

/** @brief Places and colors read on the picture the team chose (proposal B of
 * the HUD design system, nickname in title), in pixels of the 480 x 270
 * picture. */
constexpr int BAND_TOP = 246;
constexpr int BAND_BOTTOM = 269;
constexpr int MIDDLE_ROW = 257;
constexpr int PICTURE_WIDTH = 480;
constexpr sf::IntRect FIRST_CHIP({6, 249}, {11, 10});
constexpr sf::IntRect THIRD_CHIP({246, 249}, {11, 10});
constexpr sf::Vector2i FIRST_NAME(20, 250);
constexpr sf::Vector2i FIRST_WORD_END(113, 250);
constexpr sf::Vector2i FIRST_SMALL_WORD_END(113, 251);
constexpr sf::Vector2i FIRST_SCORE_END(113, 260);
constexpr sf::Vector2i FIRST_ZEROS_END(73, 260);
constexpr std::array FIRST_LIVES{sf::Vector2i(6, 260), sf::Vector2i(23, 260),
                                 sf::Vector2i(40, 260)};
constexpr sf::Vector2i FIRST_LIVES_COUNT(24, 260);
constexpr sf::IntRect FIRST_PLUG({6, 260}, {8, 8});
constexpr std::array FIRST_FRAME_CORNERS{std::pair(2, 246), std::pair(117, 246),
                                         std::pair(2, 269),
                                         std::pair(117, 269)};
constexpr std::array RULE_COLUMNS{119, 239, 359};
constexpr int RULE_TOP = 250;
constexpr int RULE_HEIGHT = 16;
constexpr sf::IntRect FIRST_LIFE_SOURCE({0, 0}, {16, 8});
constexpr sf::IntRect THIRD_LIFE_SOURCE({36, 0}, {16, 8});
constexpr sf::IntRect LOST_LIFE_SOURCE({72, 0}, {16, 8});
constexpr sf::IntRect FOURTH_MARKER_SOURCE({27, 10}, {7, 7});
constexpr sf::Color FIRST_LIGHT(127, 230, 255);
constexpr sf::Color THIRD_COLOR(126, 194, 58);
constexpr sf::Color DIMMED(38, 48, 94);
constexpr sf::Color DIMMED_TEXT(143, 162, 230);
constexpr sf::Color LEADING_ZEROS(74, 90, 168);
constexpr sf::Color WARNING(255, 179, 107);
constexpr sf::Color LOCAL_FRAME(238, 241, 255);
constexpr const char* PLUG_ICON = "icons/phosphor/plugs_bold.png";
constexpr std::uint8_t PLAYER_ONE = 1;
constexpr std::uint8_t PLAYER_THREE = 3;
constexpr std::uint8_t PLAYER_FOUR = 4;
constexpr std::uint8_t NO_PLAYER = 0;
constexpr std::uint8_t PAST_THE_LAST_PLAYER = 5;
constexpr std::uint32_t SCORE = 12400;
constexpr std::uint32_t NINE_DIGIT_SCORE = 123456789;
constexpr const char* LARGEST_SHOWN_SCORE_TEXT = "9999999";
constexpr std::uint8_t TWO_LIVES = 2;
constexpr std::uint8_t THREE_LIVES = 3;
constexpr std::uint8_t FOUR_LIVES = 4;
constexpr std::uint8_t FIVE_LIVES = 5;
constexpr std::uint8_t LAST_LIFE = 1;
constexpr std::uint8_t NO_LIFE = 0;
constexpr std::size_t LIVES_SHOWN_AT_MOST = 3;
constexpr std::size_t NAME_AND_SCORE = 2;
/** @brief Nicknames of 16 and 13 characters, the first the longest the
 * protocol carries, the second holding characters of two bytes. */
constexpr const char* LONGEST_NICKNAME = "CAPTAIN_STARDUST";
constexpr const char* NICKNAME_WITH_ACCENTS = "ÉLODIE_ÉTOILE";
/** @brief What the room of 93 pixels of the first row keeps of them: 11
 * characters of 8 pixels; 5 once RESPAWN (7 characters of 6 pixels on the
 * recording canvas) and its gap of 4 take their part. */
constexpr const char* LONGEST_NICKNAME_CUT = "CAPTAIN_STA";
constexpr const char* LONGEST_NICKNAME_BESIDE_A_WORD = "CAPTA";
constexpr const char* NICKNAME_WITH_ACCENTS_CUT = "ÉLODIE_ÉTOI";

}  // namespace rtype::client::testing
