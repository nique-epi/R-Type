#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>
#include "PacketHeader.hpp"

namespace rtype::network::testing {

/**
 * @returns A header of version 0 that carries SOME_SESSION_TOKEN and
 * SOME_SEQUENCE, every other field at its default.
 */
PacketHeader someSessionHeader();

/**
 * @returns The 22 bytes of a default header: RTYP, then zeros.
 */
std::vector<std::byte> defaultHeaderBytes();

/**
 * @returns The bytes of a default header followed by @p messageBytes.
 */
std::vector<std::byte> datagramOf(
    std::initializer_list<unsigned int> messageBytes);

}  // namespace rtype::network::testing
