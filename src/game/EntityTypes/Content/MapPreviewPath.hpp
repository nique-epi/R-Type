#pragma once

#include <string>
#include <string_view>

namespace rtype::game {

/** @returns The path of the preview named @p name in a resource and in a
 * content pack: identifiers::MAP_PREVIEW_FOLDER, a slash, @p name and
 * identifiers::MAP_PREVIEW_EXTENSION. */
[[nodiscard]] std::string mapPreviewPathOf(std::string_view name);

}  // namespace rtype::game
