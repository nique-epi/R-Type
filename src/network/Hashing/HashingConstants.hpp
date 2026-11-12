#pragma once

#include <cstddef>
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

}  // namespace rtype::network
