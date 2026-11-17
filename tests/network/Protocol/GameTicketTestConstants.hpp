#pragma once

#include <string_view>

/** @brief Texts shaped almost like the text of SOME_TICKET: the prefix of a
 * session token, uppercase digits, 63 digits, 65 digits, and a `g` in place of
 * the last digit. */
constexpr std::string_view TOKEN_PREFIXED_TEXT =
    "rts_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f";
constexpr std::string_view UPPERCASE_TEXT =
    "rtt_202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F";
constexpr std::string_view SHORT_TEXT =
    "rtt_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3";
constexpr std::string_view LONG_TEXT =
    "rtt_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f0";
constexpr std::string_view LETTER_PAST_F_TEXT =
    "rtt_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3g";
