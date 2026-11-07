#pragma once

#include <cstddef>

namespace rtype::network {

/** @brief The bytes of a hash of hashOf(): 256 bits. */
constexpr std::size_t HASH_BYTES = 32;

/** @brief The characters of the context of a hash: hydro_hash_CONTEXTBYTES of
 * libhydrogen. */
constexpr std::size_t HASH_CONTEXT_LENGTH = 8;

}  // namespace rtype::network
