#pragma once

#include <cstddef>
#include <string_view>
#include "SessionTokenFileConstants.hpp"

namespace rtype::client::testing {

/** @brief A folder the session file sits in, that a first keep creates. */
constexpr std::string_view NEW_SETTINGS_FOLDER = "r-type";

/** @brief One byte past what the client reads of a session file. */
constexpr std::size_t TOO_LARGE_SESSION_FILE_SIZE =
    MAXIMUM_SESSION_TOKEN_FILE_SIZE + 1;

}  // namespace rtype::client::testing
