#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief A manifest as signed, then the same with its version changed. */
constexpr std::string_view SIGNED_MANIFEST = R"({"version":"1.1.0"})";
constexpr std::string_view CHANGED_MANIFEST = R"({"version":"9.1.0"})";

}  // namespace rtype::client::testing
