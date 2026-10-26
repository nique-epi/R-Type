#pragma once

#include <cstddef>
#include <string_view>
#include <vector>
#include "IntroCue.hpp"

/** @returns How many cues play @p sound. */
[[nodiscard]] std::size_t cuesPlaying(
    const std::vector<rtype::client::IntroCue>& cues, std::string_view sound);
