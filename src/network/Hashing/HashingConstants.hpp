#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network {

/** @brief The bytes of a hash of hashOf(): 256 bits. */
constexpr std::size_t HASH_BYTES = 32;

/** @brief The characters of the context of a hash: hydro_hash_CONTEXTBYTES of
 * libhydrogen. */
constexpr std::size_t HASH_CONTEXT_LENGTH = 8;

/**
 * @brief The context of hashOf() the two secrets of secretsMatch() are hashed
 * in: exactly HASH_CONTEXT_LENGTH characters.
 */
constexpr std::string_view SECRET_COMPARISON_CONTEXT = "seccmpr1";

/**
 * @brief The bytes of the key password hashes are encrypted with, and of what
 * is kept of a password: hydro_pwhash_MASTERKEYBYTES and
 * hydro_pwhash_STOREDBYTES of libhydrogen.
 */
constexpr std::size_t PASSWORD_KEY_BYTES = 32;
constexpr std::size_t PASSWORD_HASH_BYTES = 128;

/**
 * @brief The cost of a password hash: the rounds of hydro_pwhash_create(), and
 * the memory and threads libhydrogen takes (it ignores the memory, and runs one
 * thread). 200 000 rounds take about 60 ms on an Apple M-series processor in a
 * release build, measured: enough that a password is slow to guess should the
 * database and its key both leak, little enough that a sign-in stays quick. A
 * check refuses a hash made with more rounds than these.
 */
constexpr std::uint64_t PASSWORD_HASH_ROUNDS = 200'000;
constexpr std::size_t PASSWORD_HASH_MEMORY = 0;
constexpr std::uint8_t PASSWORD_HASH_THREADS = 1;

}  // namespace rtype::network
