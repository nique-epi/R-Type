#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The path the fake server answers the pack at. */
constexpr std::string_view PACK_PATH = "/api/packs/lancer";

/** @brief Where a test cuts the first answer, and the range the resumed try
 * must then ask for (RFC 9110). */
constexpr std::size_t CUT_AFTER = 10;
constexpr std::string_view RANGE_FROM_THE_CUT = "bytes=10-";

/** @brief The requests of a download that went whole at once, and of one
 * resumed once. */
constexpr std::size_t ONE_REQUEST = 1;
constexpr std::size_t TWO_REQUESTS = 2;

/** @brief A status the server refuses with. */
constexpr int NOT_FOUND_STATUS = 404;

/** @brief How long a test waits for a download to end, and how often it
 * looks. */
constexpr std::chrono::seconds DOWNLOAD_WAIT{20};
constexpr std::chrono::milliseconds DOWNLOAD_POLL{10};

/** @brief A port of the loopback nothing listens on: the first of the
 * private range of IANA, which no test server binds. */
constexpr std::uint16_t SILENT_PORT = 49151;

}  // namespace rtype::client::testing
