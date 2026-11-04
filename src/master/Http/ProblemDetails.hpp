#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns The problem document of RFC 9457 for an error of @p status:
 * `{"status":404,"title":"Not Found"}`, @p title being the phrase of the
 * status. It names no type, so the type is `about:blank`: the status says
 * everything, and nothing tells the client why a route failed.
 */
[[nodiscard]] std::string problemDetailsText(int status,
                                             std::string_view title);

}  // namespace rtype::master
