#include "ResourceManifestReading.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <toml++/toml.hpp>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EngineException.hpp"
#include "ResourceConstants.hpp"
#include "ResourceManifest.hpp"
#include "ResourceName.hpp"

namespace rtype::engine {

namespace {

/** @returns The resource name @p node holds, checked.
 * @throws What readResourceManifest() throws for a dependency. */
std::string dependencyOf(const toml::node& node, const DataFileTable& table) {
  const std::optional<std::string> name = node.value<std::string>();
  if (!name.has_value()) {
    throw ResourceDependenciesExpectedException(
        placeOf(node.source(), table, RESOURCE_DEPENDENCIES_KEY));
  }
  requireResourceName(*name);
  return *name;
}

/** @returns The resources the `depends` key of @p table names, none when the
 * key is left out. */
std::vector<std::string> dependenciesOf(const DataFileTable& table) {
  const toml::node* const node =
      table.table.get().get(RESOURCE_DEPENDENCIES_KEY);
  if (node == nullptr) {
    return {};
  }
  const toml::array* const names = node->as_array();
  if (names == nullptr) {
    throw ResourceDependenciesExpectedException(
        placeOf(node->source(), table, RESOURCE_DEPENDENCIES_KEY));
  }
  std::vector<std::string> dependencies;
  for (const toml::node& name : *names) {
    dependencies.push_back(dependencyOf(name, table));
  }
  return dependencies;
}

}  // namespace

ResourceManifest readResourceManifest(
    const std::filesystem::path& resourceFolder) {
  const std::filesystem::path file =
      resourceFolder / RESOURCE_MANIFEST_FILE_NAME;
  const std::string source = shownPath(file);
  const std::string text = dataFileContent(file);
  const toml::table root = parseDataFile(text, source);
  const DataFileTable table = rootOf(root, source);
  refuseUnknownKeys(table, RESOURCE_MANIFEST_KEYS);
  return ResourceManifest{
      .description = textAt(table, RESOURCE_DESCRIPTION_KEY),
      .dependencies = dependenciesOf(table)};
}

}  // namespace rtype::engine
