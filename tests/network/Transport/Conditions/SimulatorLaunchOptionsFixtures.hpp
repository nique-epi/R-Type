#pragma once

#include <span>
#include <string_view>
#include "NetworkConditions.hpp"

/**
 * @returns The network the simulator options @p arguments describe, each
 * read in turn as a program reads them. Fails the running test at an
 * argument that is no simulator option.
 */
[[nodiscard]] rtype::network::NetworkConditions conditionsOf(
    std::span<const std::string_view> arguments);
