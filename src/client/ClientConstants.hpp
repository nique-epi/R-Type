#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief Module name the client writes the log lines of its launch under.
 */
constexpr std::string_view CLIENT_LOGGER_NAME = "Client";

}  // namespace rtype::client
