#pragma once

namespace rtype::client {

/** @brief A grain of the dust of the landing: the side it flies to (-1 left,
 * 1 right), how high it rises, how far it flies, the row it starts from. */
struct DustGrain {
  int side{};
  int rise{};
  int reach{};
  int row{};
};

}  // namespace rtype::client
