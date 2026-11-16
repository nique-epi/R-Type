#pragma once

#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "OneSessionAccounts.hpp"
#include "User.hpp"

/** @returns Accounts whose one session, KNOWN_SESSION_TOKEN, names ACE. */
[[nodiscard]] OneSessionAccounts accountsOfAce();

/** @returns A request carrying @p token as Bearer, none when it is empty. */
[[nodiscard]] rtype::network::ApiRequest requestWithToken(
    std::string_view token);

/** @returns An answer of ANSWER_OF_THE_ACCOUNT_STATUS whose body is the
 * nickname of @p user. */
[[nodiscard]] rtype::network::ApiResponse answerNaming(
    const rtype::network::User& user);
