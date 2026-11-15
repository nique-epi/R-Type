#pragma once

#include <cstddef>
#include <string_view>

/** @brief The paths of the routes of the accounts, and the method that closes a
 * session. */
constexpr std::string_view USERS_URL = "/api/users";
constexpr std::string_view OWN_ACCOUNT_URL = "/api/users/me";
constexpr std::string_view SESSIONS_URL = "/api/sessions";
constexpr std::string_view OWN_SESSION_URL = "/api/sessions/me";
constexpr std::string_view DELETE_METHOD_TEXT = "DELETE";

/** @brief The nickname of ACE in lowercase, and passwords ACE does not have.
 */
constexpr std::string_view ACE_IN_LOWERCASE = "ace";
constexpr std::string_view WRONG_PASSWORD = "correct horse battery stapler";
constexpr std::string_view SEVEN_BYTE_PASSWORD = "seven77";

/** @brief The members the answers of the accounts hold, and their values. */
constexpr std::string_view ACCOUNT_NICKNAME_MEMBER_NAME = "nickname";
constexpr std::string_view ROLE_MEMBER_NAME = "role";
constexpr std::string_view TOKEN_MEMBER_NAME = "token";
constexpr std::string_view PLAYER_ROLE_NAME = "player";
constexpr std::string_view ADMIN_ROLE_NAME = "admin";
constexpr std::string_view SESSION_TOKEN_START = "rts_";
constexpr std::string_view ACCOUNT_LOCATION_START = "/api/users/";
constexpr std::string_view PASSWORD_POINTER_TEXT = "/password";

/** @brief The sign-ins one address may fail, and the accounts it may create,
 * before the master refuses it. */
constexpr std::size_t ALLOWED_SIGN_IN_FAILURES = 5;
constexpr std::size_t ALLOWED_ACCOUNT_CREATIONS = 5;

/** @brief The line of master.cfg that makes ACE an admin. */
constexpr std::string_view ACE_ADMIN_LINE = "admin ACE";

/** @brief What the journal writes of a refused sign-in. */
constexpr std::string_view REFUSED_SIGN_IN_TEXT = "sign-in as ACE from ";
