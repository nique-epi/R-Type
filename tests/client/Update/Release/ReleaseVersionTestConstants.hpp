#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief A version whose first number is the largest std::uint32_t, then one
 * whose first number is one past it. */
constexpr std::string_view LARGEST_VERSION_NUMBER_TEXT = "4294967295.0.0";
constexpr std::string_view NUMBER_PAST_ITS_WIDTH_TEXT = "4294967296.0.0";

}  // namespace rtype::client::testing
