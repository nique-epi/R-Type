#pragma once

#include <httplib.h>
#include <string>
#include <string_view>
#include "RouteCall.hpp"
#include "UserCredentials.hpp"

/** @returns ACE and its password. */
[[nodiscard]] rtype::network::UserCredentials aceCredentials();

/** @returns ACE with @p password. */
[[nodiscard]] rtype::network::UserCredentials aceWith(
    std::string_view password);

/** @returns A `POST` of @p credentials to @p path, over HTTPS. */
[[nodiscard]] RouteCall credentialsCall(
    std::string_view path, const rtype::network::UserCredentials& credentials);

/** @returns The read of the account of the session of @p token, over HTTPS.
 */
[[nodiscard]] RouteCall readOwnUserCall(std::string_view token);

/** @returns The closing of the session of @p token, over HTTPS. */
[[nodiscard]] RouteCall deleteSessionCall(std::string_view token);

/** @returns The text of the member @p member of the JSON object @p answer
 * holds; empty when it holds none. */
[[nodiscard]] std::string memberIn(const httplib::Result& answer,
                                   std::string_view member);
