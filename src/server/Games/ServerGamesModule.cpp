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
#include "EventBus.hpp"
#include "FarewellSender.hpp"
#include "GameInstance.hpp"
#include "GameInstanceParts.hpp"
#include "GameSettings.hpp"
#include "Games.hpp"
#include "GamesSettings.hpp"
#include "HostedGamesWatch.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "Logger.hpp"
#include "MasterMessageConstants.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "PlayerDatagrams.hpp"
#include "RtypeGameModules.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SettingValue.hpp"
#include "StateEncoding.hpp"
#include "TextSettingDefinition.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "VanillaBreaches.hpp"
#include "WaveCatalog.hpp"
#include "WavePlan.hpp"

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
 * @brief Writes in the journal the waves file at @p file, read as @p catalog,
 * with the count of the difficulties it describes.
 */
void logWavesFile(const logging::Logger& logger,
                  const std::filesystem::path& file,
                  const WaveCatalog& catalog) {
  logger.info("waves file ", engine::genericText(file), ": ",
              catalog.difficulties().size(), " difficulties");
}

/**
 * @brief Writes in the journal the difficulty the games play and whether they
 * are vanilla, then one line per reason when they are not.
 */
void logHowGamesPlay(const logging::Logger& logger, std::string_view difficulty,
                     const std::vector<std::string_view>& breaches) {
  logger.info("games play ", difficulty,
              breaches.empty() ? ", vanilla" : ", not vanilla");
  for (const std::string_view breach : breaches) {
    logger.info("not vanilla: ", breach);
  }
}

/**
 * @throws NoJoinableGameException when @p gameCount is 0.
 * @throws TooManyGamesException when it is above MAXIMUM_HOSTED_GAMES, the most
 * games a heartbeat lists.
 */
void requireGameCount(std::size_t gameCount) {
  if (gameCount == 0) {
    throw NoJoinableGameException();
  }
  if (gameCount > network::MAXIMUM_HOSTED_GAMES) {
    throw TooManyGamesException(gameCount, network::MAXIMUM_HOSTED_GAMES);
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
  context.provide(games_);
  context.provide(hostedGames_);
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { playGames(); });
  context.addWork(OUTPUT_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    games_.dispatchEvents();
                    watch_->publishIfChanged(games_);
                  });
}

void ServerGamesModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  seedSetting_ = &configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{SEED_SETTING_NAME},
      .defaultValue = settings_.seed,
      .lowest = 0,
      .highest = HIGHEST_SEED,
      .help = "Seed the random draws of the first game start from, or none",
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
  requireGameCount(settings_.gameCount);
  const std::string_view difficulty = DEFAULT_DIFFICULTY;
  const WaveCatalog catalog = WaveCatalog::fromFile(settings_.wavesFile);
  logWavesFile(logger_, settings_.wavesFile, catalog);
  const WavePlan& waves = catalog.planOf(difficulty);
  const std::optional<std::uint32_t> seed = seedOf(*seedSetting_);
  const auto statesPerSecond =
      static_cast<std::size_t>(stateRateSetting_->integer().value_or(
          static_cast<std::int64_t>(settings_.statesPerSecond)));
  const StateEncoding encoding = stateEncodingOf(*stateEncodingSetting_);
  const auto inputDelayTicks =
      static_cast<std::size_t>(inputDelaySetting_->integer().value_or(
          static_cast<std::int64_t>(settings_.inputDelayTicks)));
  const std::vector<std::string_view> breaches =
      vanillaBreaches(catalog.fingerprint(), seed, inputDelayTicks);
  logHowGamesPlay(logger_, difficulty, breaches);
  auto& watch = watch_.emplace(hostedGames_, std::string{difficulty});
  for (std::size_t game = 0; game < settings_.gameCount; ++game) {
    auto events = std::make_unique<engine::EventBus>();
    std::unique_ptr<GameInstance> created = createGame(
        *events,
        GameSettings{.seed = seed ? *seed + (static_cast<std::uint32_t>(game) *
                                             GAME_SEED_STRIDE)
                                  : std::random_device{}(),
                     .waves = waves,
                     .statesPerSecond = statesPerSecond,
                     .stateEncoding = encoding,
                     .inputDelayTicks = inputDelayTicks,
                     .difficulty = std::string{difficulty},
                     .vanilla = breaches.empty()});
    watch.watch(*events);
    games_.add(std::move(events), std::move(created));
  }
}

void ServerGamesModule::stop() {
  FarewellSender farewell{*sender_, [](engine::Duration pause) {
                            std::this_thread::sleep_for(pause);
                          }};
  farewell.sayGoodbyeTo(games_);
  if (watch_.has_value()) {
    watch_->forget();
  }
}

std::unique_ptr<GameInstance> ServerGamesModule::createGame(
    engine::EventBus& events, GameSettings settings) {
  return std::make_unique<GameInstance>(
      GameInstanceParts{
          .send = std::bind_front(&IDatagramSender::queueDatagram, sender_),
          .settings = std::move(settings),
          .events = events,
          .clock = clock_},
      rtypeGameModules(*results_));
}

void ServerGamesModule::playGames() {
  for (const std::unique_ptr<GameInstance>& game : games_.all()) {
    keepDatagramsOf(game->roster(), tickDatagrams_->players(),
                    datagramsOfGame_);
    game->onTick(datagramsOfGame_);
    for (const ConnectedPlayer& departed : game->takeDepartedPlayers()) {
      admission_->forget(departed.endpoint, departed.sessionToken);
    }
  }
}

}  // namespace rtype::server
