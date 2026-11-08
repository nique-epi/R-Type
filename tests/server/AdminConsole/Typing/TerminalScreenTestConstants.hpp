#pragma once

#include <string_view>

/** @brief What erases the row of the prompt (VT100 "\r\x1b[K"). */
constexpr std::string_view ERASED_ROW = "\r\x1b[K";
