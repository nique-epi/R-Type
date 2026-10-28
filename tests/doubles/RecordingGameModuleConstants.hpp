#pragma once

#include <string_view>

namespace rtype::server::testing {

constexpr std::string_view INSTALL_CALL = "install";
constexpr std::string_view START_CALL = "start";
constexpr std::string_view STOP_CALL = "stop";
constexpr std::string_view JOINED_CALL = "joined";
constexpr std::string_view LEAVING_CALL = "leaving";

}  // namespace rtype::server::testing
