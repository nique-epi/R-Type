#pragma once

#include <chrono>
#include <string_view>

namespace rtype::client {

/** @brief What the sign-in dialog writes while it waits for a code, while a
 * player validates it, and once signed in. */
constexpr std::string_view SIGN_IN_TITLE = "SIGN IN";
constexpr std::string_view ASKING_CODE_LINE = "ASKING THE MASTER FOR A CODE...";
constexpr std::string_view ENTER_CODE_AT_START = "ENTER IT AT ";
constexpr std::string_view VALID_FOR_START = " - ";
constexpr std::string_view MINUTES_END = " MIN";
constexpr std::string_view LAST_MINUTE_TEXT = " - UNDER A MINUTE";
constexpr std::string_view SIGNED_IN_TITLE = "SIGNED IN";
constexpr std::string_view OPENING_ACCOUNT_LINE = "OPENING YOUR ACCOUNT...";

/** @brief What it writes when the link ends without a session. */
constexpr std::string_view DENIED_TITLE = "CODE REFUSED";
constexpr std::string_view DENIED_LINE =
    "THE CONNECTION WAS REFUSED ON THE SITE.";
constexpr std::string_view EXPIRED_TITLE = "CODE EXPIRED";
constexpr std::string_view EXPIRED_LINE = "ASK FOR A NEW CODE.";
constexpr std::string_view REFUSED_TITLE = "TOO MANY CODES";
constexpr std::string_view REFUSED_LINE = "WAIT A WHILE, THEN ASK AGAIN.";
constexpr std::string_view UNREACHABLE_TITLE = "NO MASTER";
constexpr std::string_view UNREACHABLE_LINE = "THE MASTER DOES NOT ANSWER.";
constexpr std::string_view SESSION_REFUSED_TITLE = "SIGN IN FAILED";
constexpr std::string_view SESSION_REFUSED_LINE =
    "THE MASTER REFUSED THE SESSION.";

/** @brief The button that asks for another code once one ended. */
constexpr const char* NEW_CODE_LABEL = "NEW CODE";

/** @brief The schemes left out of the address the dialog writes, so the
 * player reads `RTYPE.APP/LINK`. */
constexpr std::string_view SECURE_SCHEME = "https://";
constexpr std::string_view PLAIN_SCHEME = "http://";

/** @brief The unit the dialog counts the life of a code in: a minute, so its
 * menu is built again once a minute at most, and keeps the selection of the
 * player in between. */
constexpr std::chrono::minutes SIGN_IN_COUNT_UNIT{1};

}  // namespace rtype::client
