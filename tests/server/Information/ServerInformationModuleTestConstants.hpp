#pragma once

#include <string_view>

namespace rtype::server::testing {

/** @brief The name a server announces while server-name is left as it is,
 * from server.cfg.example. */
constexpr std::string_view NAME_OF_A_SERVER_LEFT_AS_SHIPPED = "R-Type server";

/** @brief A line that names the server, and the name it gives. */
constexpr std::string_view NAME_THE_SERVER_LYON_LINE = "set server-name Lyon";
constexpr std::string_view LYON = "Lyon";

/** @brief A line that gives the server a name of 33 bytes, one too many. */
constexpr std::string_view NAME_TOO_LONG_LINE =
    "set server-name abcdefghijklmnopqrstuvwxyz0123456";

}  // namespace rtype::server::testing
