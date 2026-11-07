#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace rtype::client {

/** @brief Where a content pack is downloaded from, and how large it is. */
struct PackDownloadTarget {
  /** @brief The address the player joined: the pack comes from the server it
   * chose, never from an address a datagram names. */
  std::string host;
  std::uint16_t port{};
  /** @brief The path of the pack, as network::contentPackPathOf() writes it. */
  std::string path;
  std::size_t size{};
};

}  // namespace rtype::client
