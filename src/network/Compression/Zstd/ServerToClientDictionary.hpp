#pragma once

#include <cstdint>
#include <span>

namespace rtype::network {

/**
 * @brief The zstd dictionary both programs compress and decompress the body of
 * a datagram with: the bytes of assets/compression/server_to_client.dict,
 * compiled into the program by CMake so a client and a server can never hold
 * two versions of it.
 *
 * The file is produced by tests/tools/compression/train_dictionary.py from
 * datagram bodies the server sent with a session token in real games, and is
 * never edited by hand. A frame made with it does not name it, so changing it
 * changes the protocol version.
 */
[[nodiscard]] std::span<const std::uint8_t> serverToClientDictionary();

}  // namespace rtype::network
