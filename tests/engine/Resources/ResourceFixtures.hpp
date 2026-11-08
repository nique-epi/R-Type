#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ResourcePlaces.hpp"
#include "TemporaryFolder.hpp"

/** @returns @p views as texts, in their order: the lines and names the
 * functions under test read. */
[[nodiscard]] std::vector<std::string> textsOf(
    std::span<const std::string_view> views);

/** @returns The assets folder and the resource folder of the test content,
 * inside @p root. */
[[nodiscard]] rtype::engine::ResourcePlaces placesIn(
    const TemporaryFolder& root);
