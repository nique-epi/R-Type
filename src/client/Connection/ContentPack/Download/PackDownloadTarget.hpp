#pragma once

#include <cstddef>
#include <string>

namespace rtype::client {

/** @brief Where a file is downloaded from, and how large it is. */
struct PackDownloadTarget {
  /** @brief `http://<host>:<port>` of the server the player joined (a pack
   * comes from the server it chose, never from an address a datagram names),
   * or the https origin of the releases. */
  std::string origin;
  /** @brief The path of the file on that origin, as
   * network::contentPackPathOf() writes it for a pack. */
  std::string path;
  std::size_t size{};
  /** @brief Whether a redirection is followed, to another host too: true for
   * the releases, whose files GitHub serves from another host. */
  bool followsRedirects{};
};

}  // namespace rtype::client
