#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief The master on this machine, on the default port of a master: what
 * the server map reads when no setting names another. */
constexpr std::string_view MASTER_ON_THIS_MACHINE = "http://127.0.0.1:8080";

/** @brief The master of another machine, which a player names with
 * `master-url`. */
constexpr std::string_view REMOTE_MASTER_URL = "http://203.0.113.5:8080";

/** @brief A master behind TLS, which the client cannot call. */
constexpr std::string_view SECURE_MASTER_URL = "https://master.example.org";

/** @brief A master on port 0, which no server listens on. */
constexpr std::string_view PORT_ZERO_MASTER_URL = "http://127.0.0.1:0";

}  // namespace rtype::client::testing
