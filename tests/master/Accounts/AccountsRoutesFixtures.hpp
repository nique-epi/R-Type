#pragma once

#include <httplib.h>
#include <string>
#include <string_view>
#include "AccountCredentials.hpp"
#include "RouteCall.hpp"

/** @returns ACE and its password. */
[[nodiscard]] rtype::network::AccountCredentials aceCredentials();

/** @returns ACE with @p password. */
[[nodiscard]] rtype::network::AccountCredentials aceWith(
    std::string_view password);

/** @returns A `POST` of @p credentials to @p path, over HTTPS. */
[[nodiscard]] RouteCall credentialsCall(
    std::string_view path,
    const rtype::network::AccountCredentials& credentials);

/** @returns The read of the account of the session of @p token, over HTTPS.
 */
[[nodiscard]] RouteCall readOwnAccountCall(std::string_view token);

/** @returns The closing of the session of @p token, over HTTPS. */
[[nodiscard]] RouteCall closeSessionCall(std::string_view token);

/** @returns The text of the member @p member of the JSON object @p answer
 * holds; empty when it holds none. */
[[nodiscard]] std::string memberIn(const httplib::Result& answer,
                                   std::string_view member);
