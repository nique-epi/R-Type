#pragma once

#include "Endpoint.hpp"

/** @returns The address of the player the sorter admits in most tests. */
[[nodiscard]] rtype::network::Endpoint player();

/** @returns The address of a sender the sorter never admitted. */
[[nodiscard]] rtype::network::Endpoint stranger();

/** @returns The address of a second player, next to player(). */
[[nodiscard]] rtype::network::Endpoint secondPlayer();
