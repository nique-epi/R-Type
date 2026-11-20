#include "BackgroundReading.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <ratio>
#include <span>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "AssetIds.hpp"
#include "BackgroundDescription.hpp"
#include "BackgroundFileConstants.hpp"
#include "BackgroundText.hpp"
#include "ClientException.hpp"
#include "DataFilePlace.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "DecimalRange.hpp"
#include "DurationRange.hpp"
#include "EmissionGroupDescription.hpp"
#include "ImageContent.hpp"
#include "LayerContent.hpp"
#include "LayerDescription.hpp"
#include "LayerMotion.hpp"
#include "MapContent.hpp"
#include "MapLayerKind.hpp"
#include "ParticleContent.hpp"
#include "ParticleSprite.hpp"
#include "ParticleTrail.hpp"
#include "TimeConstants.hpp"
#include "TrailStop.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p table holds a key @p key. */
bool holds(const engine::DataFileTable& table, std::string_view key) {
  return table.table.get().contains(key);
}

/** @returns The flag @p key of @p table holds, as engine::flagAt() reads it,
 * or false when the table leaves the key out. */
bool flagOrFalseAt(const engine::DataFileTable& table, std::string_view key) {
  return holds(table, key) && engine::flagAt(table, key);
}

/** @returns Where @p table starts, for an error about the table as a whole,
 * named by its dotted name. */
engine::DataFilePlace placeOfTable(const engine::DataFileTable& table) {
  const toml::source_region& region = table.table.get().source();
  return engine::DataFilePlace{.file = std::string{table.source},
                               .line = region.begin.line,
                               .column = region.begin.column,
                               .key = table.name};
}

/** @returns The number @p key of @p table holds, as engine::decimalAt()
 * reads it, or nothing when the table leaves the key out. */
std::optional<double> optionalDecimalAt(const engine::DataFileTable& table,
                                        std::string_view key,
                                        engine::DecimalRange range) {
  if (!holds(table, key)) {
    return std::nullopt;
  }
  return engine::decimalAt(table, key, range);
}

/** @returns The number @p key of @p table holds, as a float; throws as
 * engine::decimalAt(). */
float floatAt(const engine::DataFileTable& table, std::string_view key,
              engine::DecimalRange range) {
  return static_cast<float>(engine::decimalAt(table, key, range));
}

/** @returns The number @p key of @p table holds, as a float, or 0 when the
 * table leaves the key out. */
float floatOrZeroAt(const engine::DataFileTable& table, std::string_view key,
                    engine::DecimalRange range) {
  return static_cast<float>(optionalDecimalAt(table, key, range).value_or(0.0));
}

/** @returns The range @p key of @p table holds, as engine::decimalRangeAt()
 * reads it, or nothing when the table leaves the key out. */
std::optional<engine::DecimalRange> optionalRangeAt(
    const engine::DataFileTable& table, std::string_view key,
    engine::DecimalRange bounds) {
  if (!holds(table, key)) {
    return std::nullopt;
  }
  return engine::decimalRangeAt(table, key, bounds);
}

/** @returns The duration of @p milliseconds, which may hold a fraction. */
engine::Duration durationOf(double milliseconds) {
  return std::chrono::duration_cast<engine::Duration>(
      std::chrono::duration<double, std::milli>(milliseconds));
}

/** @returns The durations the range @p key of @p table holds, in
 * milliseconds; throws as engine::decimalRangeAt(). */
DurationRange durationRangeAt(const engine::DataFileTable& table,
                              std::string_view key,
                              engine::DecimalRange bounds) {
  const engine::DecimalRange milliseconds =
      engine::decimalRangeAt(table, key, bounds);
  return DurationRange{.shortest = durationOf(milliseconds.lowest),
                       .longest = durationOf(milliseconds.highest)};
}

/** @returns The color of the palette @p key of @p table names; throws as
 * engine::choiceAt(), listing PALETTE_NAMES. */
sf::Color colorAt(const engine::DataFileTable& table, std::string_view key) {
  return PALETTE_COLORS.at(engine::choiceAt(table, key, PALETTE_NAMES));
}

/** @returns The color @p key of @p table names, as colorAt() reads it, or
 * nothing when the table leaves the key out. */
std::optional<sf::Color> optionalColorAt(const engine::DataFileTable& table,
                                         std::string_view key) {
  if (!holds(table, key)) {
    return std::nullopt;
  }
  return colorAt(table, key);
}

/** @returns The asset id @p key of @p table holds. @throws
 * InconsistentBackgroundException when the text is not an asset id, and what
 * engine::textAt() throws. */
std::string assetIdAt(const engine::DataFileTable& table,
                      std::string_view key) {
  std::string assetId = engine::textAt(table, key);
  if (!isValidAssetId(assetId)) {
    throw InconsistentBackgroundException(engine::placeOfKey(table, key),
                                          ASSET_ID_RULE);
  }
  return assetId;
}

/** @returns The tables of the array @p key of @p parent, as engine::tablesAt()
 * reads them. @throws TooManyBackgroundTablesException when there are more
 * than @p most. */
std::vector<engine::DataFileTable> tablesUpTo(
    const engine::DataFileTable& parent, std::string_view key,
    std::size_t most) {
  std::vector<engine::DataFileTable> tables = engine::tablesAt(parent, key);
  if (tables.size() > most) {
    throw TooManyBackgroundTablesException(engine::placeOfKey(parent, key),
                                           most);
  }
  return tables;
}

/** @returns The motion `motion` of @p layer describes, every key checked; a
 * layer without it stays where it is. */
LayerMotion motionOf(const engine::DataFileTable& layer) {
  const std::optional<engine::DataFileTable> motion =
      engine::optionalTableAt(layer, LAYER_MOTION_KEY);
  if (!motion.has_value()) {
    return LayerMotion{};
  }
  engine::refuseUnknownKeys(*motion, MOTION_KEYS);
  return LayerMotion{
      .parallax = floatOrZeroAt(*motion, PARALLAX_KEY, PARALLAX_RANGE),
      .autoscroll = floatOrZeroAt(*motion, AUTOSCROLL_KEY, AUTOSCROLL_RANGE),
      .repeat = floatOrZeroAt(*motion, REPEAT_KEY, REPEAT_RANGE),
      .warps = flagOrFalseAt(*motion, WARPS_KEY)};
}

/** @returns The image @p image describes, every key checked. */
ImageContent imageOf(const engine::DataFileTable& image) {
  engine::refuseUnknownKeys(image, IMAGE_KEYS);
  return ImageContent{
      .file = assetIdAt(image, FILE_KEY),
      .topLeft = {floatOrZeroAt(image, IMAGE_X_KEY, PLACE_RANGE),
                  floatOrZeroAt(image, IMAGE_Y_KEY, PLACE_RANGE)}};
}

/** @returns The layer of an LDtk project @p table describes, every key
 * checked, holding what @p kind says. */
MapContent mapContentOf(const engine::DataFileTable& table, MapLayerKind kind) {
  engine::refuseUnknownKeys(table, PROJECT_LAYER_KEYS);
  return MapContent{.project = assetIdAt(table, PROJECT_KEY),
                    .layer = engine::textAt(table, PROJECT_LAYER_KEY),
                    .tileset = assetIdAt(table, TILESET_KEY),
                    .kind = kind};
}

/** @returns Where the particles @p particles describes are born: its
 * `area`, every key checked. */
sf::FloatRect areaOf(const engine::DataFileTable& particles) {
  const engine::DataFileTable area = engine::tableAt(particles, AREA_KEY);
  engine::refuseUnknownKeys(area, AREA_KEYS);
  return sf::FloatRect{{floatAt(area, AREA_LEFT_KEY, PLACE_RANGE),
                        floatAt(area, AREA_TOP_KEY, PLACE_RANGE)},
                       {floatAt(area, AREA_WIDTH_KEY, SIZE_RANGE),
                        floatAt(area, AREA_HEIGHT_KEY, SIZE_RANGE)}};
}

/** @returns The sprite @p sprite describes, every key checked. */
ParticleSprite spriteOf(const engine::DataFileTable& sprite) {
  engine::refuseUnknownKeys(sprite, SPRITE_KEYS);
  return ParticleSprite{
      .file = assetIdAt(sprite, FILE_KEY),
      .frames = holds(sprite, FRAMES_KEY)
                    ? static_cast<std::size_t>(engine::wholeNumberAt(
                          sprite, FRAMES_KEY, FRAMES_RANGE))
                    : DEFAULT_FRAMES,
      .weight = optionalDecimalAt(sprite, WEIGHT_KEY, WEIGHT_RANGE)
                    .value_or(DEFAULT_WEIGHT),
      .anchor = {floatOrZeroAt(sprite, ANCHOR_X_KEY, PLACE_RANGE),
                 floatOrZeroAt(sprite, ANCHOR_Y_KEY, PLACE_RANGE)},
      .framesPerSecond = optionalRangeAt(sprite, FRAMES_PER_SECOND_KEY,
                                         FRAMES_PER_SECOND_RANGE)};
}

/** @returns The sprites `sprites` of @p particles lists, in the order of the
 * file; none when it leaves the key out. */
std::vector<ParticleSprite> spritesOf(const engine::DataFileTable& particles) {
  std::vector<ParticleSprite> sprites;
  if (!holds(particles, SPRITES_KEY)) {
    return sprites;
  }
  for (const engine::DataFileTable& sprite :
       tablesUpTo(particles, SPRITES_KEY, MOST_PARTICLE_SPRITES)) {
    sprites.push_back(spriteOf(sprite));
  }
  return sprites;
}

/** @returns The color of a trail @p stop describes, every key checked. */
TrailStop trailStopOf(const engine::DataFileTable& stop) {
  engine::refuseUnknownKeys(stop, TRAIL_STOP_KEYS);
  return TrailStop{
      .until = floatAt(stop, TRAIL_STOP_UNTIL_KEY, TRAIL_SHARE_RANGE),
      .color = colorAt(stop, TRAIL_STOP_COLOR_KEY)};
}

/** @returns The trail `trail` of @p particles describes, every key checked,
 * or nothing when it leaves the key out. */
std::optional<ParticleTrail> trailOf(const engine::DataFileTable& particles) {
  const std::optional<engine::DataFileTable> trail =
      engine::optionalTableAt(particles, TRAIL_KEY);
  if (!trail.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*trail, TRAIL_KEYS);
  std::vector<TrailStop> stops;
  for (const engine::DataFileTable& stop :
       tablesUpTo(*trail, TRAIL_STOPS_KEY, MOST_TRAIL_STOPS)) {
    stops.push_back(trailStopOf(stop));
  }
  return ParticleTrail{
      .longest = floatAt(*trail, TRAIL_LONGEST_KEY, TRAIL_LENGTH_RANGE),
      .growth = floatAt(*trail, TRAIL_GROWTH_KEY, TRAIL_PACE_RANGE),
      .shrink = floatAt(*trail, TRAIL_SHRINK_KEY, TRAIL_PACE_RANGE),
      .stops = std::move(stops)};
}

/** @returns Whether @p particles describes a population, and not the
 * particles of a group. @throws InconsistentBackgroundException when it
 * holds both `population` and `group`, or neither. */
bool isPopulation(const engine::DataFileTable& particles) {
  const bool populated = holds(particles, POPULATION_KEY);
  if (populated == holds(particles, PARTICLE_GROUP_KEY)) {
    throw InconsistentBackgroundException(placeOfTable(particles),
                                          EMISSION_RULE);
  }
  return populated;
}

/** @returns Where the particles of @p particles are born and what they are
 * drawn with: its area, and its pixel or its sprites. @throws
 * InconsistentBackgroundException when it holds both looks, or neither. */
ParticleContent lookOf(const engine::DataFileTable& particles) {
  ParticleContent content{};
  content.area = areaOf(particles);
  content.pixel = optionalColorAt(particles, PIXEL_KEY);
  content.sprites = spritesOf(particles);
  if (content.pixel.has_value() != content.sprites.empty()) {
    throw InconsistentBackgroundException(placeOfTable(particles),
                                          PARTICLE_LOOK_RULE);
  }
  return content;
}

/** @throws InconsistentBackgroundException when the population @p particles
 * describes gives a twinkle color without a pixel that twinkles, or a pixel
 * that twinkles without that color. */
void refuseAWrongTwinkle(const engine::DataFileTable& particles) {
  const bool pixelTwinkles =
      holds(particles, PIXEL_KEY) && holds(particles, TWINKLE_STEPS_KEY);
  if (holds(particles, TWINKLE_KEY) != pixelTwinkles) {
    throw InconsistentBackgroundException(placeOfTable(particles),
                                          TWINKLE_RULE);
  }
}

/** @brief Writes into @p content what the population @p particles describes
 * holds of its own: how many particles, their twinkle and their streak. */
void readPopulation(const engine::DataFileTable& particles,
                    ParticleContent& content) {
  content.population = static_cast<std::size_t>(
      engine::wholeNumberAt(particles, POPULATION_KEY, POPULATION_RANGE));
  content.twinkle = optionalColorAt(particles, TWINKLE_KEY);
  content.twinkleSteps = engine::wholeNumberOrZeroAt(
      particles, TWINKLE_STEPS_KEY, TWINKLE_STEPS_RANGE);
  content.warpTrail = optionalColorAt(particles, WARP_TRAIL_KEY);
}

/** @returns The name of the group `group` of @p particles holds. @throws
 * InconsistentBackgroundException when none of @p groups has that name. */
std::string declaredGroupAt(const engine::DataFileTable& particles,
                            std::span<const EmissionGroupDescription> groups) {
  std::string name = engine::textAt(particles, PARTICLE_GROUP_KEY);
  if (std::ranges::find(groups, name, &EmissionGroupDescription::name) ==
      groups.end()) {
    throw InconsistentBackgroundException(
        engine::placeOfKey(particles, PARTICLE_GROUP_KEY), UNKNOWN_GROUP_RULE);
  }
  return name;
}

/** @brief Writes into @p content how the particles of a group @p particles
 * describes fly and end: their chance, speed, angle, drift, life, margin and
 * trail. The group is written by the caller. */
void readFlight(const engine::DataFileTable& particles,
                ParticleContent& content) {
  content.weight = optionalDecimalAt(particles, WEIGHT_KEY, WEIGHT_RANGE)
                       .value_or(DEFAULT_WEIGHT);
  content.speed = optionalRangeAt(particles, SPEED_KEY, SPEED_RANGE)
                      .value_or(engine::DecimalRange{});
  content.angle = optionalRangeAt(particles, ANGLE_KEY, ANGLE_RANGE)
                      .value_or(content.angle);
  content.drift = floatOrZeroAt(particles, DRIFT_KEY, SPEED_RANGE);
  if (holds(particles, LIFE_KEY)) {
    content.life = durationRangeAt(particles, LIFE_KEY, LIFE_RANGE);
  }
  content.margin = floatOrZeroAt(particles, MARGIN_KEY, SIZE_RANGE);
  content.trail = trailOf(particles);
}

/** @throws InconsistentBackgroundException when the particles of a group
 * @p content holds, read from @p particles, have no life and may not move:
 * they would never end, and hold a place of their group for good. */
void refuseEndlessParticles(const engine::DataFileTable& particles,
                            const ParticleContent& content) {
  if (!content.life.has_value() && content.speed.lowest <= 0.0) {
    throw InconsistentBackgroundException(placeOfTable(particles),
                                          ENDLESS_PARTICLE_RULE);
  }
}

/** @returns The particles @p particles describes, every key checked, whose
 * group, when they have one, is one of @p groups. */
ParticleContent particlesOf(const engine::DataFileTable& particles,
                            std::span<const EmissionGroupDescription> groups) {
  if (isPopulation(particles)) {
    engine::refuseUnknownKeys(particles, POPULATION_KEYS);
    refuseAWrongTwinkle(particles);
    ParticleContent population = lookOf(particles);
    readPopulation(particles, population);
    return population;
  }
  engine::refuseUnknownKeys(particles, EMITTED_KEYS);
  ParticleContent emitted = lookOf(particles);
  emitted.group = declaredGroupAt(particles, groups);
  readFlight(particles, emitted);
  refuseEndlessParticles(particles, emitted);
  return emitted;
}

/** @returns The content @p layer holds. @throws
 * InconsistentBackgroundException when it holds none of LAYER_CONTENT_KEYS,
 * or several. */
LayerContent contentOf(const engine::DataFileTable& layer,
                       std::span<const EmissionGroupDescription> groups) {
  const auto contents = std::ranges::count_if(
      LAYER_CONTENT_KEYS,
      [&layer](std::string_view key) { return holds(layer, key); });
  if (contents != 1) {
    throw InconsistentBackgroundException(placeOfTable(layer),
                                          LAYER_CONTENT_RULE);
  }
  if (holds(layer, IMAGE_CONTENT_KEY)) {
    return imageOf(engine::tableAt(layer, IMAGE_CONTENT_KEY));
  }
  if (holds(layer, TILE_CONTENT_KEY)) {
    return mapContentOf(engine::tableAt(layer, TILE_CONTENT_KEY),
                        MapLayerKind::Tiles);
  }
  if (holds(layer, PROP_CONTENT_KEY)) {
    return mapContentOf(engine::tableAt(layer, PROP_CONTENT_KEY),
                        MapLayerKind::Props);
  }
  return particlesOf(engine::tableAt(layer, PARTICLE_CONTENT_KEY), groups);
}

/** @returns The layer @p layer describes, every key checked. */
LayerDescription layerOf(const engine::DataFileTable& layer,
                         std::span<const EmissionGroupDescription> groups) {
  engine::refuseUnknownKeys(layer, LAYER_KEYS);
  return LayerDescription{.name = engine::textAt(layer, LAYER_NAME_KEY),
                          .motion = motionOf(layer),
                          .entrance = flagOrFalseAt(layer, LAYER_ENTRANCE_KEY),
                          .content = contentOf(layer, groups)};
}

/** @returns The layers `[[layer]]` of @p root lists, in the order of the
 * file. @throws InconsistentBackgroundException at the second layer that is
 * an entrance. */
std::vector<LayerDescription> layersOf(
    const engine::DataFileTable& root,
    std::span<const EmissionGroupDescription> groups) {
  std::vector<LayerDescription> layers;
  bool entered = false;
  for (const engine::DataFileTable& table :
       tablesUpTo(root, LAYER_TABLES_KEY, MOST_BACKGROUND_LAYERS)) {
    LayerDescription layer = layerOf(table, groups);
    if (layer.entrance && entered) {
      throw InconsistentBackgroundException(
          engine::placeOfKey(table, LAYER_ENTRANCE_KEY), ONE_ENTRANCE_RULE);
    }
    entered = entered || layer.entrance;
    layers.push_back(std::move(layer));
  }
  return layers;
}

/** @returns The emission group @p group describes, every key checked. */
EmissionGroupDescription groupOf(const engine::DataFileTable& group) {
  engine::refuseUnknownKeys(group, GROUP_KEYS);
  return EmissionGroupDescription{
      .name = engine::textAt(group, GROUP_NAME_KEY),
      .mostAlive = static_cast<std::size_t>(
          engine::wholeNumberAt(group, MOST_ALIVE_KEY, MOST_ALIVE_RANGE)),
      .first =
          engine::millisecondsAt(group, FIRST_BIRTH_KEY, FIRST_BIRTH_RANGE),
      .every =
          durationRangeAt(group, BIRTH_INTERVAL_KEY, BIRTH_INTERVAL_RANGE)};
}

/** @returns The groups `[[group]]` of @p root lists, in the order of the
 * file; none when it leaves the key out. @throws
 * InconsistentBackgroundException at the second group of a name. */
std::vector<EmissionGroupDescription> groupsOf(
    const engine::DataFileTable& root) {
  std::vector<EmissionGroupDescription> groups;
  if (!holds(root, GROUP_TABLES_KEY)) {
    return groups;
  }
  for (const engine::DataFileTable& table :
       tablesUpTo(root, GROUP_TABLES_KEY, MOST_EMISSION_GROUPS)) {
    EmissionGroupDescription group = groupOf(table);
    if (std::ranges::find(groups, group.name,
                          &EmissionGroupDescription::name) != groups.end()) {
      throw InconsistentBackgroundException(
          engine::placeOfKey(table, GROUP_NAME_KEY), SHARED_GROUP_NAME_RULE);
    }
    groups.push_back(std::move(group));
  }
  return groups;
}

/** @returns The asset id of the music @p root names; none when it leaves the
 * key out. */
std::optional<std::string> musicOf(const engine::DataFileTable& root) {
  if (!holds(root, MUSIC_KEY)) {
    return std::nullopt;
  }
  return assetIdAt(root, MUSIC_KEY);
}

}  // namespace

BackgroundDescription readBackground(const BackgroundText& text) {
  const toml::table file = engine::parseDataFile(text.text, text.source);
  const engine::DataFileTable root = engine::rootOf(file, text.source);
  engine::refuseUnknownKeys(root, BACKGROUND_ROOT_KEYS);
  std::vector<EmissionGroupDescription> groups = groupsOf(root);
  std::vector<LayerDescription> layers = layersOf(root, groups);
  return BackgroundDescription{.name = text.name,
                               .layers = std::move(layers),
                               .groups = std::move(groups),
                               .music = musicOf(root)};
}

}  // namespace rtype::client
