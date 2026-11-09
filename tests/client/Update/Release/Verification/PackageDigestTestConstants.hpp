#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief The SHA-256 of "abc" (FIPS 180-2, appendix B.1), and of nothing. */
constexpr std::string_view ABC_SHA256 =
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
constexpr std::string_view EMPTY_SHA256 =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

}  // namespace rtype::client::testing
