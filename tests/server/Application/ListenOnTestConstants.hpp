#pragma once

#include <string_view>

/**
 * @brief An address no socket can bind, which a server given it at
 * construction cannot open.
 */
constexpr std::string_view UNUSABLE_ADDRESS = "not an address";
