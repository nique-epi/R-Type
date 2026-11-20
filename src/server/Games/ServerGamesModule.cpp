#include "ServerGamesModule.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScripts.hpp"
#include "ConfigurationRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsFingerprint.hpp"
#include "ErrorText.hpp"
#include "EventBus.hpp"
#include "FarewellSender.hpp"
#include "GameInstance.hpp"
#include "GameInstanceParts.hpp"
#include "GameRules.hpp"
#include "GameSettings.hpp"
#include "GamesSettings.hpp"
#include "HostedRules.hpp"
#include "IDatagramSender.hpp"
#include "IGameEvents.hpp"
#include "IPlayerAdmission.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "LevelDirector.hpp"
#include "LevelGroundFiles.hpp"
#include "LevelPlacementFiles.hpp"
#include "Logger.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevels.hpp"
#include "MapChoice.hpp"
#include "MapPreviewPath.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "PlayerDatagrams.hpp"
#include "ProtocolConstants.hpp"
#include "RtypeGameModules.hpp"
#include "RunningGame.hpp"
#include "ServerConstants.hpp"
#include "SettingValue.hpp"
#include "SheetTextReading.hpp"
#include "StateEncoding.hpp"
#include "TextSettingDefinition.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "VanillaBreaches.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server {

namespace {

/** @returns The whole number @p setting holds, @p fallback when it holds
 * none. */
std::size_t sizeSettingOr(const engine::SettingValue& setting,
                          std::size_t fallback) {
  return static_cast<std::size_t>(
      setting.integer().value_or(static_cast<std::int64_t>(fallback)));
}

/** @returns The maps the scripts @p scripts registered (the levels given the
 * option map), in the order of the names of their levels, at most
 * MAXIMUM_LOBBY_MAPS; none without scripts. */
std::vector<MapChoice> mapsOf(game::BehaviourScripts* scripts) {
  std::vector<MapChoice> maps;
  if (scripts == nullptr) {
    return maps;
  }
  for (const std::string& name : scripts->levels().names()) {
    const game::LuauLevelEntry* const entry = scripts->levels().entryOf(name);
    if (entry != nullptr && entry->mapName.has_value() &&
        maps.size() < network::MAXIMUM_LOBBY_MAPS) {
      maps.push_back(
          MapChoice{.level = name,
                    .name = *entry->mapName,
                    .preview = entry->mapPreview.has_value()
                                   ? game::mapPreviewPathOf(*entry->mapPreview)
                                   : std::string{}});
    }
  }
  return maps;
}

/**
 * @returns The seed @p setting holds, nothing when it was cleared.
 */
std::optional<std::uint32_t> seedOf(const engine::SettingValue& setting) {
  const std::optional<std::int64_t> value = setting.integer();
  if (!value.has_value()) {
    return std::nullopt;
  }
  return static_cast<std::uint32_t>(*value);
}

/** @returns The value of the `state-encoding` setting that names @p encoding.
 */
std::string_view textOf(StateEncoding encoding) {
  switch (encoding) {
    case StateEncoding::Delta:
      return DELTA_STATE_ENCODING_TEXT;
    case StateEncoding::Full:
      return FULL_STATE_ENCODING_TEXT;
  }
  return DELTA_STATE_ENCODING_TEXT;
}

/**
 * @brief Refuses a value of the `state-encoding` setting that names no
 * encoding.
 * @throws engine::CommandRefusedException for any text but
 * DELTA_STATE_ENCODING_TEXT and FULL_STATE_ENCODING_TEXT.
 */
void requireStateEncoding(std::string_view text) {
  if (text != DELTA_STATE_ENCODING_TEXT && text != FULL_STATE_ENCODING_TEXT) {
    throw engine::CommandRefusedException(
        std::string{INVALID_STATE_ENCODING_TEXT});
  }
}

/**
 * @returns The encoding @p setting names, which requireStateEncoding()
 * checked.
 */
StateEncoding stateEncodingOf(const engine::SettingValue& setting) {
  return setting.text() == FULL_STATE_ENCODING_TEXT ? StateEncoding::Full
                                                    : StateEncoding::Delta;
}

/** @returns @p paths in the generic form, separated as engine::listText()
 * separates them, for the journal. */
std::string pathsText(std::span<const std::filesystem::path> paths) {
  std::vector<std::string> shown;
  std::ranges::transform(paths, std::back_inserter(shown),
                         logging::genericText);
  return engine::listText(shown);
}

/**
 * @brief Writes in the journal the content folders the entity sheets were
 * read from, with the count of their types.
 */
void logEntitySheets(const logging::Logger& logger,
                     std::span<const std::filesystem::path> folders,
                     const game::EntitySheets& types) {
  logger.info("entity sheets of ", pathsText(folders), ": ", types.all().size(),
              " types");
}

/**
 * @brief Writes in the journal the waves files at @p files, read as
 * @p catalog, with the count of the difficulties they describe.
 */
void logWavesFiles(const logging::Logger& logger,
                   std::span<const std::filesystem::path> files,
                   const WaveCatalog& catalog) {
  logger.info("waves files ", pathsText(files), ": ",
              catalog.difficulties().size(), " difficulties");
}

/**
 * @brief Writes in the journal the rules the game plays and whether it is
 * vanilla, then one line per reason when it is not.
 */
void logHowGamesPlay(const logging::Logger& logger,
                     const network::GameRules& rules,
                     const std::vector<std::string_view>& breaches) {
  logger.info("games play ", rules.difficulty, ", ",
              static_cast<unsigned int>(rules.lives), " lives, friendly fire ",
              rules.friendlyFire ? "on" : "off",
              breaches.empty() ? ", vanilla" : ", not vanilla");
  for (const std::string_view breach : breaches) {
    logger.info("not vanilla: ", breach);
  }
}

/** @brief Writes in the journal, for each level that names an LDtk file, how
 * many enemies it places. */
void logLevelPlacements(const logging::Logger& logger,
                        const game::LuauLevels& levels) {
  for (const std::string& name : levels.names()) {
    const game::LuauLevelEntry& entry = *levels.entryOf(name);
    if (entry.ldtkFile.has_value()) {
      logger.info("level ", name, ": ", entry.placements.size(),
                  " enemies placed by ", *entry.ldtkFile);
    }
  }
}

/** @returns The setting START_AT_BOSS_SETTING_NAME, declared in
 * @p configuration: 1 starts every game at its boss, 0 (the default) plays
 * the waves. */
const engine::SettingValue& declareStartAtBoss(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{START_AT_BOSS_SETTING_NAME},
      .defaultValue = 0,
      .lowest = 0,
      .highest = 1,
      .help = "1 starts every game at its boss, without the waves"});
}

/** @returns The setting START_AT_STAGE_SETTING_NAME, declared in
 * @p configuration: the stage every game that plays a level starts at, from
 * FIRST_LEVEL_STAGE (the default) to the most a level holds. */
const engine::SettingValue& declareStartAtStage(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{START_AT_STAGE_SETTING_NAME},
      .defaultValue = FIRST_LEVEL_STAGE,
      .lowest = FIRST_LEVEL_STAGE,
      .highest = game::MAXIMUM_LEVEL_STAGES,
      .help = "The stage every game that plays a level starts at"});
}

/** @returns The stage @p setting, START_AT_STAGE_SETTING_NAME, names. */
std::size_t startingStageOf(const engine::SettingValue& setting) {
  return static_cast<std::size_t>(
      setting.integer().value_or(static_cast<std::int64_t>(FIRST_LEVEL_STAGE)));
}

/** @returns The setting LEVEL_SETTING_NAME, declared in @p configuration:
 * the level every game plays, or nothing (the default) to play the waves. */
const engine::SettingValue& declareLevel(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{LEVEL_SETTING_NAME},
      .defaultValue = std::string{},
      .help = "Level a script registered that every game plays, or nothing "
              "to play the waves",
      .validate = {}});
}

}  // namespace

ServerGamesModule::ServerGamesModule(GamesSettings settings)
    : settings_(std::move(settings)),
      logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerGamesModule::name() const {
  return std::string{GAMES_MODULE_NAME};
}

std::vector<std::string> ServerGamesModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{STORAGE_MODULE_NAME}};
}

void ServerGamesModule::install(engine::InstallContext& context) {
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  results_ = &context.require<IResultsArchive>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  declareSettings(context.require<engine::ConfigurationRegistry>());
  context.provide(runningGame_);
  context.provide<IGameEvents>(runningGame_);
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { playGame(); });
  context.addWork(OUTPUT_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    runningGame_.events().dispatch();
                  });
}

void ServerGamesModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  seedSetting_ = &configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{SEED_SETTING_NAME},
      .defaultValue = settings_.seed,
      .lowest = 0,
      .highest = HIGHEST_SEED,
      .help = "Seed the random draws of the game start from, or none",
      .allowsNone = true});
  stateRateSetting_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{STATE_RATE_SETTING_NAME},
          .defaultValue = static_cast<std::int64_t>(settings_.statesPerSecond),
          .lowest = LOWEST_WORLD_STATES_PER_SECOND,
          .highest = HIGHEST_WORLD_STATES_PER_SECOND,
          .help = "World states sent to each player per second"});
  stateEncodingSetting_ =
      &configuration.declareText(engine::TextSettingDefinition{
          .name = std::string{STATE_ENCODING_SETTING_NAME},
          .defaultValue = std::string{textOf(DEFAULT_STATE_ENCODING)},
          .help = "How world states are written: delta or full",
          .validate = requireStateEncoding});
  inputDelaySetting_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{INPUT_DELAY_SETTING_NAME},
          .defaultValue = static_cast<std::int64_t>(settings_.inputDelayTicks),
          .lowest = LOWEST_INPUT_DELAY_TICKS,
          .highest = HIGHEST_INPUT_DELAY_TICKS,
          .help = "Ticks every player's input is held before it is applied"});
  ruleSettings_.declare(configuration);
  startAtBossSetting_ = &declareStartAtBoss(configuration);
  startAtStageSetting_ = &declareStartAtStage(configuration);
  levelSetting_ = &declareLevel(configuration);
}

void ServerGamesModule::start() {
  scripts_ = std::make_unique<game::BehaviourScripts>(
      [this](const std::string& message) { logger_.warn(message); });
  scripts_->run(game::scriptSourcesIn(settings_.contentFolders));
  if (!levelSetting_->text().empty()) {
    static_cast<void>(startOfStage(
        entryOfLevel(scripts_->levels(), levelSetting_->text()),
        levelSetting_->text(), startingStageOf(*startAtStageSetting_)));
  }
  const game::EntitySheets& types =
      types_.emplace(game::EntitySheets::fromTexts(
          game::sheetTextsInFolders(settings_.contentFolders),
          scripts_->behaviours()));
  logEntitySheets(logger_, settings_.contentFolders, types);
  game::readLevelPlacements(scripts_->levels(), settings_.contentFolders,
                            types);
  game::readLevelGrounds(scripts_->levels(), settings_.contentFolders);
  logLevelPlacements(logger_, scripts_->levels());
  runningGame_.keepEntityTypes(types);
  const WaveCatalog catalog =
      WaveCatalog::fromFiles(settings_.wavesFiles, types);
  logWavesFiles(logger_, settings_.wavesFiles, catalog);
  const std::optional<std::uint32_t> seed = seedOf(*seedSetting_);
  runningGame_.host(createGame(settingsOf(catalog, types, seed)));
}

GameSettings ServerGamesModule::settingsOf(const WaveCatalog& catalog,
                                           const game::EntitySheets& types,
                                           std::optional<std::uint32_t> seed) {
  const network::GameRules rules = ruleSettings_.rules();
  const std::size_t inputDelayTicks =
      sizeSettingOr(*inputDelaySetting_, settings_.inputDelayTicks);
  const bool startsAtBoss = startAtBossSetting_->integer().value_or(0) != 0;
  const std::string& level = levelSetting_->text();
  std::vector<MapChoice> maps =
      level.empty() ? mapsOf(scripts_.get()) : std::vector<MapChoice>{};
  const std::vector<std::string_view> breaches = vanillaBreaches(
      HostedRules{.wavesFingerprint = catalog.fingerprint(),
                  .sheetsFingerprint = entitySheetsFingerprint(types),
                  .seed = seed,
                  .inputDelayTicks = inputDelayTicks,
                  .gameRules = rules,
                  .startsAtBoss = startsAtBoss,
                  .playsALevel = !level.empty() || !maps.empty()});
  logHowGamesPlay(logger_, rules, breaches);
  return GameSettings{
      .seed = seed.has_value() ? *seed : std::random_device{}(),
      .waves = catalog.planOf(rules.difficulty),
      .types = types,
      .statesPerSecond =
          sizeSettingOr(*stateRateSetting_, settings_.statesPerSecond),
      .stateEncoding = stateEncodingOf(*stateEncodingSetting_),
      .inputDelayTicks = inputDelayTicks,
      .rules = rules,
      .vanilla = breaches.empty(),
      .startsAtBoss = startsAtBoss,
      .level = level.empty() ? std::nullopt : std::optional{level},
      .maps = std::move(maps),
      .startingStage = startingStageOf(*startAtStageSetting_),
      .scripts = scripts_.get()};
}

void ServerGamesModule::stop() {
  FarewellSender farewell{*sender_, [](engine::Duration pause) {
                            std::this_thread::sleep_for(pause);
                          }};
  farewell.sayGoodbyeTo(runningGame_.game().roster());
}

std::unique_ptr<GameInstance> ServerGamesModule::createGame(
    GameSettings settings) {
  return std::make_unique<GameInstance>(
      GameInstanceParts{
          .send = std::bind_front(&IDatagramSender::queueDatagram, sender_),
          .settings = std::move(settings),
          .events = runningGame_.events(),
          .clock = clock_},
      rtypeGameModules(*results_));
}

void ServerGamesModule::playGame() {
  GameInstance& game = runningGame_.game();
  keepDatagramsOf(game.roster(), tickDatagrams_->players(), datagramsOfGame_);
  game.onTick(datagramsOfGame_);
  for (const ConnectedPlayer& departed : game.takeDepartedPlayers()) {
    admission_->forget(departed.endpoint, departed.sessionToken);
  }
}

}  // namespace rtype::server
