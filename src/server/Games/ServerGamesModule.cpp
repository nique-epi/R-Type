#include "ServerGamesModule.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsFingerprint.hpp"
#include "EventBus.hpp"
#include "FarewellSender.hpp"
#include "GameInstance.hpp"
#include "GameInstanceParts.hpp"
#include "GameSettings.hpp"
#include "GamesSettings.hpp"
#include "HostedRules.hpp"
#include "IDatagramSender.hpp"
#include "IGameEvents.hpp"
#include "IPlayerAdmission.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "Logger.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "PlayerDatagrams.hpp"
#include "RtypeGameModules.hpp"
#include "RunningGame.hpp"
#include "ServerConstants.hpp"
#include "SettingValue.hpp"
#include "StateEncoding.hpp"
#include "TextSettingDefinition.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "VanillaBreaches.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server {

namespace {

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

/**
 * @brief Writes in the journal the entity sheets read from @p folder, with the
 * count of their types.
 */
void logEntitySheets(const logging::Logger& logger,
                     const std::filesystem::path& folder,
                     const game::EntitySheets& types) {
  logger.info("entity sheets ", logging::genericText(folder), ": ",
              types.all().size(), " types");
}

/**
 * @brief Writes in the journal the waves file at @p file, read as @p catalog,
 * with the count of the difficulties it describes.
 */
void logWavesFile(const logging::Logger& logger,
                  const std::filesystem::path& file,
                  const WaveCatalog& catalog) {
  logger.info("waves file ", logging::genericText(file), ": ",
              catalog.difficulties().size(), " difficulties");
}

/**
 * @brief Writes in the journal the difficulty the game plays and whether it is
 * vanilla, then one line per reason when it is not.
 */
void logHowGamesPlay(const logging::Logger& logger, std::string_view difficulty,
                     const std::vector<std::string_view>& breaches) {
  logger.info("games play ", difficulty,
              breaches.empty() ? ", vanilla" : ", not vanilla");
  for (const std::string_view breach : breaches) {
    logger.info("not vanilla: ", breach);
  }
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
}

void ServerGamesModule::start() {
  const game::EntitySheets& types = types_.emplace(
      game::EntitySheets::fromFolder(settings_.entitySheetsFolder));
  logEntitySheets(logger_, settings_.entitySheetsFolder, types);
  runningGame_.keepEntityTypes(types);
  const WaveCatalog catalog = WaveCatalog::fromFile(settings_.wavesFile, types);
  logWavesFile(logger_, settings_.wavesFile, catalog);
  const std::optional<std::uint32_t> seed = seedOf(*seedSetting_);
  runningGame_.host(createGame(settingsOf(catalog, types, seed)));
}

GameSettings ServerGamesModule::settingsOf(const WaveCatalog& catalog,
                                           const game::EntitySheets& types,
                                           std::optional<std::uint32_t> seed) {
  const std::string_view difficulty = DEFAULT_DIFFICULTY;
  const auto statesPerSecond =
      static_cast<std::size_t>(stateRateSetting_->integer().value_or(
          static_cast<std::int64_t>(settings_.statesPerSecond)));
  const StateEncoding encoding = stateEncodingOf(*stateEncodingSetting_);
  const auto inputDelayTicks =
      static_cast<std::size_t>(inputDelaySetting_->integer().value_or(
          static_cast<std::int64_t>(settings_.inputDelayTicks)));
  const std::vector<std::string_view> breaches = vanillaBreaches(
      HostedRules{.wavesFingerprint = catalog.fingerprint(),
                  .sheetsFingerprint = entitySheetsFingerprint(types),
                  .seed = seed,
                  .inputDelayTicks = inputDelayTicks});
  logHowGamesPlay(logger_, difficulty, breaches);
  return GameSettings{.seed = seed.has_value() ? *seed : std::random_device{}(),
                      .waves = catalog.planOf(difficulty),
                      .types = types,
                      .statesPerSecond = statesPerSecond,
                      .stateEncoding = encoding,
                      .inputDelayTicks = inputDelayTicks,
                      .difficulty = std::string{difficulty},
                      .vanilla = breaches.empty()};
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
