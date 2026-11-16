#pragma once

#include "AssetFileLookup.hpp"
#include "TextureLookup.hpp"

/** @returns A lookup that finds no texture. */
[[nodiscard]] rtype::client::TextureLookup noTexture();

/** @returns A lookup that finds the files of the assets folder of the
 * repository. */
[[nodiscard]] rtype::client::AssetFileLookup filesOfTheAssets();

/** @returns A lookup that names a file that does not exist, whatever the
 * id. */
[[nodiscard]] rtype::client::AssetFileLookup filesNowhere();
