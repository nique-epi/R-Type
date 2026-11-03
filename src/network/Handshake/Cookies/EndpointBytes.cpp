#include "EndpointBytes.hpp"
#include <span>
#include "ByteWriter.hpp"
#include "Endpoint.hpp"

namespace rtype::network {

void writeEndpoint(ByteWriter& writer, const Endpoint& endpoint) {
  writer.writeBytes(std::as_bytes(std::span{endpoint.address}));
  writer.writeUint16(endpoint.port);
}

}  // namespace rtype::network
