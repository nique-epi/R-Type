#include "ServerGamesModule.hpp"
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "FarewellSender.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameInstance.hpp"
#include "GameSettings.hpp"
#include "Games.hpp"
#include "GamesSettings.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "WaveCatalog.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

ServerGamesModule::ServerGamesModule(GamesSettings settings)
    : settings_(std::move(settings)),
      logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerGamesModule::name() const {
  return std::string{GAMES_MODULE_NAME};
}

std::vector<std::string> ServerGamesModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}};
}

void ServerGamesModule::install(engine::InstallContext& context) {
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  context.provide(games_);
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { playGames(); });
  context.addWork(OUTPUT_STAGE,
                  [this](engine::Duration elapsed) { deliverEvents(elapsed); });
}

void ServerGamesModule::start() {
  std::unique_ptr<GameInstance> game = createGame();
  auto broadcaster = std::make_unique<GameEventBroadcaster>(wiringFor(*game));
  games_.add(std::move(game));
  broadcaster_ = std::move(broadcaster);
}

void ServerGamesModule::stop() {
  FarewellSender farewell{*sender_, [](engine::Duration pause) {
                            std::this_thread::sleep_for(pause);
                          }};
  farewell.sayGoodbyeTo(games_);
  broadcaster_.reset();
}

WavePlan ServerGamesModule::readWaves() const {
  const WaveCatalog catalog = WaveCatalog::fromFile(settings_.wavesFile);
  WavePlan waves = catalog.planOf(DEFAULT_DIFFICULTY);
  logger_.info("waves file ", settings_.wavesFile.string(), ": ",
               catalog.difficulties().size(), " difficulties, games play ",
               DEFAULT_DIFFICULTY);
  return waves;
}

std::unique_ptr<GameInstance> ServerGamesModule::createGame() {
  IDatagramSender* const sender = sender_;
  return std::make_unique<GameInstance>(
      [sender](network::OutgoingDatagram datagram) {
        sender->queueDatagram(std::move(datagram));
      },
      GameSettings{.seed = settings_.seed.value_or(std::random_device{}()),
                   .waves = readWaves()},
      events_);
}

GameEventBroadcaster::Wiring ServerGamesModule::wiringFor(GameInstance& game) {
  IDatagramSender* const sender = sender_;
  return GameEventBroadcaster::Wiring{
      .events = events_,
      .timers = timers_,
      .recipients = [&game] { return game.roster().endpoints(); },
      .headerFor =
          [&game](const network::Endpoint& recipient) {
            return game.nextSessionHeaderFor(recipient);
          },
      .send =
          [sender](network::OutgoingDatagram datagram) {
            sender->queueDatagram(std::move(datagram));
          }};
}

void ServerGamesModule::playGames() {
  for (const std::unique_ptr<GameInstance>& game : games_.all()) {
    game->onTick(tickDatagrams_->players());
    for (const ConnectedPlayer& departed : game->takeDepartedPlayers()) {
      admission_->forget(departed.endpoint, departed.sessionToken);
    }
  }
}

void ServerGamesModule::deliverEvents(engine::Duration elapsed) {
  timers_.advance(elapsed);
  events_.dispatch();
}

}  // namespace rtype::server
