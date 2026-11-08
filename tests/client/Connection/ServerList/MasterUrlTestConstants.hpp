#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief A master named by its IPv4 address and port, and its url. */
constexpr std::string_view ADDRESSED_MASTER_URL = "http://10.0.0.2:8080";
constexpr std::string_view ADDRESSED_MASTER_ADDRESS = "10.0.0.2";
constexpr std::uint16_t ADDRESSED_MASTER_PORT = 8080;

}  // namespace rtype::client::testing
