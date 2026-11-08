#pragma once

#include "WaveCatalog.hpp"

namespace rtype::server::testing {

/** @returns The waves the games play once the example resource is started
 * over the assets of the sources: wavesFilesOf() its content folders, read
 * with their entity sheets and the behaviours of their scripts. */
[[nodiscard]] WaveCatalog wavesWithTheExampleResource();

/** @returns The waves the games play from the assets of the sources alone. */
[[nodiscard]] WaveCatalog wavesOfTheAssets();

}  // namespace rtype::server::testing
