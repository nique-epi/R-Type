#include "ContentFolders.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <ranges>
#include <span>
#include <string>
#include <system_error>
#include <vector>
#include "AssetFolder.hpp"
#include "DataFileReading.hpp"
#include "EngineException.hpp"
#include "ResourceConstants.hpp"
#include "ResourceManifest.hpp"
#include "ResourceManifestReading.hpp"
#include "ResourcePlaces.hpp"

namespace rtype::engine {

namespace {

/** @brief The resources a walk of the dependencies has met: those started, in
 * their order, and those it is inside of, from the first line down. */
struct ResourceWalk {
  std::reference_wrapper<const ResourcePlaces> places;
  std::vector<std::string> inside;
  std::vector<std::string> started;
  std::vector<std::filesystem::path> folders;
};

/** @returns The manifest of the resource @p name.
 * @throws ResourceNotFoundException when its folder holds none. */
ResourceManifest manifestOf(const ResourceWalk& walk, const std::string& name) {
  const std::filesystem::path folder = walk.places.get().resourceFolder / name;
  std::error_code error;
  if (!std::filesystem::is_regular_file(folder / RESOURCE_MANIFEST_FILE_NAME,
                                        error)) {
    throw ResourceNotFoundException(name, shownPath(folder));
  }
  return readResourceManifest(folder);
}

/** @throws ResourceDependencyCycleException when @p name is one of the
 * resources @p walk is inside of. */
void refuseCircle(const ResourceWalk& walk, const std::string& name) {
  const auto first = std::ranges::find(walk.inside, name);
  if (first == walk.inside.end()) {
    return;
  }
  std::vector<std::string> chain{first, walk.inside.end()};
  chain.push_back(name);
  throw ResourceDependencyCycleException(chain);
}

/** @brief A resource the walk is to visit: first to lay its dependencies,
 * then, once they are laid, to lay itself. */
struct ResourceStep {
  std::string name;
  bool dependenciesLaid{false};
};

/** @brief Enters @p name: reads its manifest, then plans to lay its
 * dependencies, in their order, before it. */
void enter(ResourceWalk& walk, const std::string& name,
           std::vector<ResourceStep>& steps) {
  refuseCircle(walk, name);
  const ResourceManifest manifest = manifestOf(walk, name);
  walk.inside.push_back(name);
  steps.push_back(ResourceStep{.name = name, .dependenciesLaid = true});
  for (const auto& dependency : manifest.dependencies | std::views::reverse) {
    steps.push_back(
        ResourceStep{.name = dependency, .dependenciesLaid = false});
  }
}

/** @brief Lays @p name, whose dependencies are laid. */
void lay(ResourceWalk& walk, const std::string& name) {
  walk.inside.pop_back();
  walk.started.push_back(name);
  walk.folders.push_back(walk.places.get().resourceFolder / name);
}

/** @brief Starts @p name in @p walk, after the resources it depends on. */
void start(ResourceWalk& walk, const std::string& name) {
  std::vector<ResourceStep> steps{
      ResourceStep{.name = name, .dependenciesLaid = false}};
  while (!steps.empty()) {
    const ResourceStep step = steps.back();
    steps.pop_back();
    if (std::ranges::find(walk.started, step.name) != walk.started.end()) {
      continue;
    }
    if (step.dependenciesLaid) {
      lay(walk, step.name);
    } else {
      enter(walk, step.name, steps);
    }
  }
}

}  // namespace

ResourcePlaces locateResourcePlaces() {
  const std::filesystem::path assetFolder = locateAssetFolder();
  return ResourcePlaces{
      .assetFolder = assetFolder,
      .resourceFolder = assetFolder.parent_path() / RESOURCE_FOLDER_NAME};
}

std::vector<std::filesystem::path> contentFoldersOf(
    const ResourcePlaces& places, std::span<const std::string> names) {
  ResourceWalk walk{.places = places,
                    .inside = {},
                    .started = {},
                    .folders = {places.assetFolder}};
  for (const std::string& name : names) {
    start(walk, name);
  }
  return walk.folders;
}

}  // namespace rtype::engine
