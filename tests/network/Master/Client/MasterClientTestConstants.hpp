#pragma once

#include <cstdint>
#include <string_view>

/** @brief The statuses the master refuses with in the tests of the client. */
constexpr int EXPECTED_UNAUTHORIZED_STATUS = 401;
constexpr int EXPECTED_CONFLICT_STATUS = 409;
constexpr int EXPECTED_BAD_REQUEST_STATUS = 400;

/** @brief The players of parisFinishedGame(), a row each on the master. */
constexpr int PLAYERS_OF_THE_PARIS_GAME = 2;

/** @brief What the master says of a heartbeat sent before the registration. */
constexpr std::string_view UNREGISTERED_DETAIL_TEXT =
    "PUT /api/servers/me first";

/** @brief A url of the master over TLS, which the client does not speak. */
constexpr std::string_view HTTPS_MASTER_URL = "https://master.example.org";

/** @brief A url of the master whose port is past 65535. */
constexpr std::string_view PORT_TOO_HIGH_MASTER_URL = "http://127.0.0.1:99999";

/** @brief An id no server of the tests has. */
constexpr std::int64_t UNKNOWN_ID = 999;

/** @brief A status that tells no refusal was caught. */
constexpr int NO_REFUSAL = 0;
