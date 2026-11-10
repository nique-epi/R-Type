#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief A feed as signed, then the same with its version changed. */
constexpr std::string_view SIGNED_FEED = R"({"version":"1.1.0"})";
constexpr std::string_view CHANGED_FEED = R"({"version":"9.1.0"})";

}  // namespace rtype::client::testing
