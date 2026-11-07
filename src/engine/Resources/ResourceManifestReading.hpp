#pragma once

#include <filesystem>
#include "ResourceManifest.hpp"

namespace rtype::engine {

/**
 * @brief Reads the RESOURCE_MANIFEST_FILE_NAME of @p resourceFolder, as
 * strictly as the other data files: a key it does not know, a missing
 * description or a `depends` that is not a list of resource names stops the
 * read with the file, the line and the key.
 * @returns The manifest, named after the folder.
 * @throws engine::UnreadableDataFileException when the file cannot be read,
 * what parseDataFile(), refuseUnknownKeys() and textAt() throw,
 * ResourceDependenciesExpectedException, and InvalidResourceNameException for
 * a dependency that is not a resource name.
 */
[[nodiscard]] ResourceManifest readResourceManifest(
    const std::filesystem::path& resourceFolder);

}  // namespace rtype::engine
