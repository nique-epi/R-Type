#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The master the form opens on, as its url and as its fields show
 * it. */
constexpr std::string_view FORM_MASTER_URL = "http://10.0.0.2:8080";
constexpr std::string_view FORM_MASTER_ADDRESS = "10.0.0.2";
constexpr std::uint16_t FORM_MASTER_PORT = 8080;
constexpr std::u32string_view FORM_MASTER_PORT_TEXT = U"8080";

/** @brief A master the master-url setting names by host, which the address
 * field cannot show. */
constexpr std::string_view HOST_MASTER_URL = "http://master.example.org:8080";

/** @brief A port the player types over the one read, as typed and as
 * kept. */
constexpr std::u32string_view TYPED_MASTER_PORT_TEXT = U"9090";
constexpr std::uint16_t TYPED_MASTER_PORT = 9090;

}  // namespace rtype::client::testing
