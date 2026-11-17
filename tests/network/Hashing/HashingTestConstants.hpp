#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief A secret a program holds, and what a caller may present for it. */
constexpr std::string_view HELD_SECRET = "c0ffee-held-by-the-program";
constexpr std::string_view SAME_LENGTH_GUESS = "c0ffee-held-by-the-progran";
constexpr std::string_view START_OF_THE_SECRET = "c0ffee-held";
constexpr std::string_view NO_SECRET;

/** @brief Keys of the passwords, as `openssl rand -hex 32` writes them, and
 * texts that are no key. */
constexpr std::string_view FIRST_KEY =
    "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff";
constexpr std::string_view SECOND_KEY =
    "ffeeddccbbaa99887766554433221100ffeeddccbbaa99887766554433221100";
constexpr std::string_view KEY_IN_CAPITALS =
    "00A1B2C3D4E5F60718293A4B5C6D7E8F00A1B2C3D4E5F60718293A4B5C6D7E8F";
constexpr std::size_t SECOND_BYTE = 1;
constexpr std::uint8_t SECOND_BYTE_OF_KEY = 0xA1;
constexpr std::string_view KEY_ONE_BYTE_SHORT =
    "00112233445566778899aabbccddeeff00112233445566778899aabbccddee";
constexpr std::string_view KEY_ONE_BYTE_TOO_LONG =
    "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff00";
constexpr std::string_view KEY_WITH_A_LETTER_PAST_F =
    "00112233445566778899aabbccddeeff00112233445566778899aabbccddeegg";

/** @brief Two passwords a player may choose. */
constexpr std::string_view A_PASSWORD = "correct horse battery staple";
constexpr std::string_view ANOTHER_PASSWORD = "correct horse battery stapler";
