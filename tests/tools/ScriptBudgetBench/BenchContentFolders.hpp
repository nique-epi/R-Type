#pragma once

#include <filesystem>
#include <vector>

namespace rtype::tools {

/** @returns The folders of this repository the bench reads its sheets and its
 * script from, whichever folder it is launched in. */
[[nodiscard]] std::vector<std::filesystem::path> benchContentFolders();

}  // namespace rtype::tools
