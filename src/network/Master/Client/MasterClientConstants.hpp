#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::network {

/**
 * @brief How long a call waits to connect to the master, then for each read
 * and write of its answer: a master that does not answer within them is
 * unreachable, where cpp-httplib would wait 300 seconds by default.
 */
constexpr std::chrono::seconds MASTER_CONNECTION_TIMEOUT{5};
constexpr std::chrono::seconds MASTER_TRANSFER_TIMEOUT{5};

/**
 * @brief How many pages listServers() reads at most: a master that always
 * answers a full page would otherwise be read for ever. Ten pages of
 * MAXIMUM_PAGE_SIZE are a thousand servers, far more than the master lists.
 */
constexpr std::int64_t MAXIMUM_LISTED_SERVER_PAGES = 10;

/** @brief What a query string starts with, joins its parameters with, and
 * joins a parameter and its value with: `?limit=100&offset=0`. */
constexpr std::string_view QUERY_START = "?";
constexpr std::string_view QUERY_PARAMETER_SEPARATOR = "&";
constexpr std::string_view QUERY_VALUE_SEPARATOR = "=";

}  // namespace rtype::network
