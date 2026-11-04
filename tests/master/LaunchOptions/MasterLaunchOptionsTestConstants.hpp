#pragma once

#include <string_view>

/** @brief A port given after `-p`, and the line it must force. */
constexpr std::string_view GIVEN_PORT = "8081";
constexpr std::string_view FORCED_PORT_LINE = "set port 8081";

/** @brief The lowest and the highest port a master accepts. */
constexpr std::string_view LOWEST_ACCEPTED_PORT = "1";
constexpr std::string_view HIGHEST_ACCEPTED_PORT = "65535";

/** @brief The ports just outside the ones a master accepts. */
constexpr std::string_view PORT_BELOW_THE_RANGE = "0";
constexpr std::string_view PORT_ABOVE_THE_RANGE = "65536";

/** @brief A file given after `--database`, and the line it must force. */
constexpr std::string_view GIVEN_DATABASE = "/tmp/master.db";
constexpr std::string_view FORCED_DATABASE_LINE = "set database /tmp/master.db";

/** @brief An option of the client the master does not take. */
constexpr std::string_view CLIENT_ADDRESS_OPTION = "-h";
constexpr std::string_view CLIENT_ADDRESS = "127.0.0.1";
