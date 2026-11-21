#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The account a player signs the title in as, and its token. */
constexpr std::uint64_t TITLE_ACCOUNT_ID = 7;
constexpr std::string_view TITLE_ACCOUNT_NICKNAME = "ACE";
constexpr std::string_view TITLE_ACCOUNT_TOKEN =
    "rts_fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";

/** @brief What the corners of the title write: SIGN IN for a guest; the
 * caption of the balance and SIGN OUT for a player signed in. */
constexpr std::string_view SIGN_IN_TEXT = "SIGN IN";
constexpr std::string_view SIGN_OUT_TEXT = "SIGN OUT";
constexpr std::string_view BALANCE_CAPTION_TEXT = "KOPECKS";

/**
 * @brief A pixel of the interface inside SIGN IN, in the top-right corner of
 * the mockup: six pixels left of the right side of the safe margin (456),
 * six pixels under its top (24), on a chip 16 pixels tall.
 */
constexpr sf::Vector2i SIGN_IN_CORNER_PIXEL(450, 30);

}  // namespace rtype::client::testing
