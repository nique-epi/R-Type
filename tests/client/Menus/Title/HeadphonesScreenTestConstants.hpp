#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief Where the mockup places the logo (36 × 30) and the middles of the
 * tops of its two lines, on the interface of 480 × 270. */
constexpr sf::IntRect HEADPHONES_LOGO_AREA{{222, 103}, {36, 30}};
constexpr sf::Vector2i HEADPHONES_LINE_PLACE{240, 145};
constexpr sf::Vector2i HEADPHONES_SMALL_LINE_PLACE{240, 159};

/** @brief The size of a fill the recording canvas writes for each plotted
 * pixel. */
constexpr sf::Vector2i ONE_PLOTTED_PIXEL{1, 1};

/** @brief The two lines the mockup writes. */
constexpr std::string_view HEADPHONES_RECOMMENDED = "HEADPHONES RECOMMENDED";
constexpr std::string_view FOR_THE_BEST_EXPERIENCE = "FOR THE BEST EXPERIENCE";

/** @brief A pixel of the interface the tests click at: any pixel leaves. */
constexpr sf::Vector2i HEADPHONES_CLICK_PIXEL{100, 100};

/** @brief 38 frames of 16 ms make 608 ms, past the 0.6 s fade in. */
constexpr std::chrono::milliseconds PAST_THE_HEADPHONES_FADE_IN{608};

/** @brief 224 frames of 16 ms make 3584 ms, short of the 3.6 s the screen
 * lasts; one more frame reaches 3600 ms. */
constexpr std::chrono::milliseconds JUST_BEFORE_THE_INTRO{3584};

/** @brief 100 more frames make 608 + 1600 = 2208 ms, short of the fade out
 * at 2600 ms. */
constexpr std::chrono::milliseconds MOST_OF_THE_HOLD{1600};

/** @brief 63 frames make 1008 ms, while the screen stays; a leave then ends
 * at 1008 + 600 + 400 = 2008 ms. */
constexpr std::chrono::milliseconds WHILE_THE_HEADPHONES_STAY{1008};

/** @brief 62 more frames make 1008 + 992 = 2000 ms, short of 2008 ms; one
 * more frame reaches 2016 ms. */
constexpr std::chrono::milliseconds JUST_BEFORE_A_LEAVE_ENDS{992};

/** @brief 75 more frames make 1008 + 1200 = 2208 ms: past where a leave
 * would have ended, short of the 3.6 s the screen lasts. */
constexpr std::chrono::milliseconds PAST_WHERE_A_LEAVE_ENDS{1200};

/** @brief Half the fade in: a release then leaves from half the light, so the
 * leave ends at 300 − 600 × 0.5 + 600 + 400 = 1000 ms. */
constexpr std::chrono::milliseconds HALFWAY_INTO_THE_HEADPHONES_FADE_IN{300};

/** @brief 43 frames make 300 + 688 = 988 ms, short of 1000 ms; one more frame
 * reaches 1004 ms, still short of the 1300 ms a leave from full light would
 * end at. */
constexpr std::chrono::milliseconds JUST_BEFORE_A_HALF_LEAVE_ENDS{688};

/** @brief 188 more frames make 1008 + 3008 = 4016 ms, past the 3.6 s the
 * screen lasts. */
constexpr std::chrono::milliseconds HELD_PAST_THE_INTRO{3008};

/** @brief 181 frames make 2896 ms, 296 ms into the fade out of 600 ms that
 * starts at 2600 ms: the ink then covers 296 / 600 of the picture, an alpha of
 * 255 × 296 / 600 = 125.8, so 126. */
constexpr std::chrono::milliseconds HALFWAY_INTO_THE_FADE_OUT{2896};
constexpr std::uint8_t HALFWAY_INK_ALPHA = 126;

/** @brief 219 frames make 3504 ms, in the ink after the fade out ends at 3200
 * ms; 6 more frames reach 3600 ms, when the intro is due. */
constexpr std::chrono::milliseconds IN_THE_LAST_INK{3504};
constexpr std::chrono::milliseconds TO_THE_END_OF_THE_INK{96};

}  // namespace rtype::client::testing
