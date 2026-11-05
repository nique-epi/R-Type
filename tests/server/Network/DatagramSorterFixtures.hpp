#pragma once

#include <cstddef>
#include "Endpoint.hpp"

/** @returns The address of the player the sorter admits in most tests. */
[[nodiscard]] rtype::network::Endpoint player();

/** @returns The address of a sender the sorter never admitted. */
[[nodiscard]] rtype::network::Endpoint stranger();

/** @returns The address of a second player, next to player(). */
[[nodiscard]] rtype::network::Endpoint secondPlayer();

/** @returns The address of a second stranger, next to stranger(). */
[[nodiscard]] rtype::network::Endpoint otherStranger();

/**
 * @returns The address of stranger @p number of a flood from many strangers:
 * the address of the sorting tests, a port each.
 */
[[nodiscard]] rtype::network::Endpoint floodingStranger(std::size_t number);
