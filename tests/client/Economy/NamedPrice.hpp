#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief An item by its name in the catalogue, and the price expected. */
struct NamedPrice {
  std::string_view name;
  std::uint32_t price{};
};

}  // namespace rtype::client::testing
