#include "ServerGamesModule.hpp"
#include <cstddef>
#include <cstdint>
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
#include "EventBus.hpp"
#include "FarewellSender.hpp"
#include "GameInstance.hpp"
#include "GameInstanceParts.hpp"
#include "GameSettings.hpp"
#include "Games.hpp"
#include "GamesSettings.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "PlayerDatagrams.hpp"
#include "RtypeGameModules.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SettingValue.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
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
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { playGames(); });
  context.addWork(OUTPUT_STAGE,
                  [this](engine::Duration elapsed) { deliverEvents(elapsed); });
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
  inputDelaySetting_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{INPUT_DELAY_SETTING_NAME},
          .defaultValue = static_cast<std::int64_t>(settings_.inputDelayTicks),
          .lowest = LOWEST_INPUT_DELAY_TICKS,
          .highest = HIGHEST_INPUT_DELAY_TICKS,
          .help = "Ticks every player's input is held before it is applied"});
}

void ServerGamesModule::start() {
  if (settings_.gameCount == 0) {
    throw NoJoinableGameException();
  }
  const std::string_view difficulty = DEFAULT_DIFFICULTY;
  const WavePlan waves = readWaves(difficulty);
  const std::optional<std::uint32_t> seed = seedOf(*seedSetting_);
  const auto statesPerSecond =
      static_cast<std::size_t>(stateRateSetting_->integer().value_or(
          static_cast<std::int64_t>(settings_.statesPerSecond)));
  const auto inputDelayTicks =
      static_cast<std::size_t>(inputDelaySetting_->integer().value_or(
          static_cast<std::int64_t>(settings_.inputDelayTicks)));
  for (std::size_t game = 0; game < settings_.gameCount; ++game) {
    auto events = std::make_unique<engine::EventBus>();
    std::unique_ptr<GameInstance> created = createGame(
        *events,
        GameSettings{.seed = seed ? *seed + (static_cast<std::uint32_t>(game) *
                                             GAME_SEED_STRIDE)
                                  : std::random_device{}(),
                     .waves = waves,
                     .statesPerSecond = statesPerSecond,
                     .inputDelayTicks = inputDelayTicks,
                     .difficulty = std::string{difficulty}});
    games_.add(std::move(events), std::move(created));
  }
}

void ServerGamesModule::stop() {
  FarewellSender farewell{*sender_, [](engine::Duration pause) {
                            std::this_thread::sleep_for(pause);
                          }};
  farewell.sayGoodbyeTo(games_);
}

WavePlan ServerGamesModule::readWaves(std::string_view difficulty) const {
  const WaveCatalog catalog = WaveCatalog::fromFile(settings_.wavesFile);
  WavePlan waves = catalog.planOf(difficulty);
  logger_.info("waves file ", engine::genericText(settings_.wavesFile), ": ",
               catalog.difficulties().size(), " difficulties, games play ",
               difficulty);
  return waves;
}

std::unique_ptr<GameInstance> ServerGamesModule::createGame(
    engine::EventBus& events, GameSettings settings) {
  return std::make_unique<GameInstance>(
      GameInstanceParts{
          .send = std::bind_front(&IDatagramSender::queueDatagram, sender_),
          .settings = std::move(settings),
          .events = events,
          .timers = timers_,
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

void ServerGamesModule::deliverEvents(engine::Duration elapsed) {
  timers_.advance(elapsed);
  games_.dispatchEvents();
}

}  // namespace rtype::server
