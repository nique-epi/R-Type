#pragma once

#include <cstddef>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>

namespace rtype::client {

/** @returns The body of a GET of @p path on RELEASES_ORIGIN, its redirections
 * followed, when the answer is 200 and holds at most @p limit bytes; nothing
 * otherwise, or once @p stop was asked. It waits RELEASE_CHECK_TIMEOUT at most
 * to connect, and as long for each read. */
[[nodiscard]] std::optional<std::string> fetchReleaseFile(
    std::string_view path, std::size_t limit, const std::stop_token& stop);

}  // namespace rtype::client
