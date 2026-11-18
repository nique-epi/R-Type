#include "LayerBuilding.hpp"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "BackgroundConstants.hpp"
#include "BackgroundDescription.hpp"
#include "BuiltLayers.hpp"
#include "ClientException.hpp"
#include "EmissionGroupDescription.hpp"
#include "EmissionGroupMember.hpp"
#include "ILayerContent.hpp"
#include "ImageContent.hpp"
#include "ImageLayer.hpp"
#include "LayerDescription.hpp"
#include "LayerSources.hpp"
#include "LdtkProjectFile.hpp"
#include "Logger.hpp"
#include "MapContent.hpp"
#include "MapLayer.hpp"
#include "MapPlane.hpp"
#include "ParticleContent.hpp"
#include "ParticleLayer.hpp"
#include "ParticleLayerParts.hpp"

namespace rtype::client {

namespace {

using MembersByGroup =
    std::map<std::string, std::vector<EmissionGroupMember>, std::less<>>;
using ProjectsByFile = std::map<std::string, LdtkProjectFile, std::less<>>;

/** @brief What building the layers of one background goes through: where
 * its files lie, the seeds it hands out, the journal, and the layers of
 * particles met so far, by the group they name. */
struct LayerWorkshop {
  std::reference_wrapper<const LayerSources> sources;
  std::mt19937 seeds;
  logging::Logger logger;
  MembersByGroup members;
  ProjectsByFile projects;
};

/** @returns A seed for one layer or one group, drawn from the seeds of
 * @p workshop. */
std::uint32_t nextSeed(LayerWorkshop& workshop) {
  return static_cast<std::uint32_t>(workshop.seeds());
}

/** @returns The LDtk project the asset id @p project names, read on its
 * first use by @p workshop. @throws LdtkProjectException when it cannot be
 * read. */
const LdtkProjectFile& projectOf(const std::string& project,
                                 LayerWorkshop& workshop) {
  return workshop.projects
      .try_emplace(project, workshop.sources.get().fileOf(project))
      .first->second;
}

std::unique_ptr<ILayerContent> contentOf(
    const ImageContent& image, const LayerDescription& layer,
    [[maybe_unused]] LayerWorkshop& workshop) {
  return std::make_unique<ImageLayer>(image, layer.motion.repeat);
}

/** @returns The layer that shows the maps @p maps names, taken from its
 * project; a layer that shows nothing, told in the journal under the name of
 * @p layer, when the project or its layer cannot be read. */
std::unique_ptr<ILayerContent> contentOf(const MapContent& maps,
                                         const LayerDescription& layer,
                                         LayerWorkshop& workshop) {
  try {
    return std::make_unique<MapLayer>(
        projectOf(maps.project, workshop).plane(maps.layer, maps.kind),
        maps.tileset);
  } catch (const LdtkProjectException& error) {
    workshop.logger.error("the layer ", layer.name,
                          " of a background shows nothing: ", error.what());
    return std::make_unique<MapLayer>(MapPlane{}, maps.tileset);
  }
}

std::unique_ptr<ILayerContent> contentOf(const ParticleContent& particles,
                                         const LayerDescription& layer,
                                         LayerWorkshop& workshop) {
  auto built = std::make_unique<ParticleLayer>(
      ParticleLayerParts{.description = particles,
                         .repeat = layer.motion.repeat,
                         .seed = nextSeed(workshop)});
  if (!particles.group.empty()) {
    workshop.members[particles.group].push_back(
        EmissionGroupMember{.emitter = *built, .weight = particles.weight});
  }
  return built;
}

}  // namespace

BuiltLayers layersOf(const BackgroundDescription& background,
                     const LayerSources& sources) {
  LayerWorkshop workshop{
      .sources = sources,
      .seeds = std::mt19937{sources.seed},
      .logger = logging::Logger{std::string{BACKGROUND_LOGGER_NAME}},
      .members = {},
      .projects = {}};
  BuiltLayers built;
  for (const LayerDescription& layer : background.layers) {
    built.contents.push_back(std::visit(
        [&layer, &workshop](const auto& content) {
          return contentOf(content, layer, workshop);
        },
        layer.content));
  }
  for (const EmissionGroupDescription& group : background.groups) {
    built.groups.emplace_back(group, std::move(workshop.members[group.name]),
                              nextSeed(workshop));
  }
  return built;
}

}  // namespace rtype::client
