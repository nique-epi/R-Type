#pragma once

#include <string>
#include <string_view>

namespace rtype::network {

/** @returns @p text with its ASCII letters in lowercase, as a header name or a
 * scheme of HTTP is compared (RFC 9110). */
[[nodiscard]] std::string lowercaseOf(std::string_view text);

}  // namespace rtype::network
