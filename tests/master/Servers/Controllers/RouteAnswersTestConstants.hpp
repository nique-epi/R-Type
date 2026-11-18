#pragma once

#include <cstdint>
#include <string_view>

/** @brief The session the accounts of the tests know, the account it names,
 * and a token of the same shape no session has. */
constexpr std::string_view KNOWN_SESSION_TOKEN =
    "rts_00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff";
constexpr std::string_view UNKNOWN_SESSION_TOKEN =
    "rts_ffeeddccbbaa99887766554433221100ffeeddccbbaa99887766554433221100";
constexpr std::int64_t SESSION_ACCOUNT_ID = 7;
constexpr std::string_view SESSION_ACCOUNT_NICKNAME = "ACE";

/** @brief The status of the answer of the tests once it named the account. */
constexpr int ANSWER_OF_THE_ACCOUNT_STATUS = 200;

/** @brief The challenge a refusal for no session carries in
 * `WWW-Authenticate` (RFC 6750). */
constexpr std::string_view BEARER_CHALLENGE = "Bearer";
