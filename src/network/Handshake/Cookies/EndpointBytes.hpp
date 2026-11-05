#pragma once

#include "ByteWriter.hpp"
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @brief Writes the address of @p endpoint, as its text, then its port: the
 * bytes the keyed hashes of a sender start from, so a cookie and a row of the
 * rate limit name a sender the same way.
 */
void writeEndpoint(ByteWriter& writer, const Endpoint& endpoint);

}  // namespace rtype::network
