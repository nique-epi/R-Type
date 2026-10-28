#include "GamesModule.hpp"
#include <cstddef>
#include <exception>
#include <filesystem>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameEventConstants.hpp"
#include "GameInstance.hpp"
#include "GameList.hpp"
#include "GameListSettings.hpp"
#include "GameSettings.hpp"
#include "IServerLink.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"
#include "WaveCatalog.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The waves of DEFAULT_DIFFICULTY, read from @p waveFile; the file and
 * how many difficulties it describes are logged.
 * @throws ServerException when the file cannot be read, does not describe the
 * difficulties as the server expects them, or has no DEFAULT_DIFFICULTY: the
 * message names the file, the line and the key.
 */
WavePlan wavesOf(const std::filesystem::path& waveFile,
                 const logging::Logger& logger) {
  const WaveCatalog catalog = WaveCatalog::fromFile(waveFile);
  WavePlan waves = catalog.planOf(DEFAULT_DIFFICULTY);
  logger.info("waves file ", waveFile.string(), ": ",
              catalog.difficulties().size(), " difficulties, games play ",
              DEFAULT_DIFFICULTY);
  return waves;
}

/** @returns What a game calls to send a datagram: send() of @p link. */
GameInstance::Send sendingThrough(IServerLink& link) {
  return [&link](network::OutgoingDatagram datagram) {
    link.send(std::move(datagram));
  };
}

/**
 * @brief Makes @p link forget the address of every player of @p game whose
 * session closed in this tick.
 */
void forgetDepartedPlayers(GameInstance& game, IServerLink& link) {
  for (const ConnectedPlayer& departed : game.takeDepartedPlayers()) {
    link.forget(departed.endpoint, departed.sessionToken);
  }
}

/** @returns How many players the games of @p games hold. */
std::size_t connectedPlayerCount(const GameList& games) {
  std::size_t count = 0;
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    count += game->roster().connected().size();
  }
  return count;
}

/**
 * @brief Queues a Disconnection for every player of @p games, under its
 * session header, then has @p link send them at once.
 */
void sendDisconnections(const GameList& games, IServerLink& link) {
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    for (ConnectedPlayer* player : game->roster().connected()) {
      link.send(sessionDatagram(*player, network::Disconnection{}));
    }
  }
  link.sendQueuedAfterRun();
}

}  // namespace

GamesModule::GamesModule(GameListSettings settings)
    : settings_(std::move(settings)),
      logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string GamesModule::name() const { return std::string{GAMES_MODULE_NAME}; }

std::vector<std::string> GamesModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}};
}

void GamesModule::install(engine::InstallContext& context) {
  link_ = &context.require<IServerLink>();
  received_ = &context.require<const ServerApplication::ReceivedDatagrams>();
  context.provide(games_);
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { playTick(); });
  context.addWork(OUTPUT_STAGE,
                  [this](engine::Duration elapsed) { deliverEvents(elapsed); });
}

void GamesModule::start() {
  try {
    open();
  } catch (...) {
    close();
    throw;
  }
}

void GamesModule::stop() {
  if (connectedPlayerCount(games_) > 0) {
    sayGoodbye();
  }
  close();
}

void GamesModule::open() {
  WavePlan waves = wavesOf(settings_.waveFile, logger_);
  events_ = std::make_unique<engine::EventBus>();
  timers_ = std::make_unique<engine::TimerScheduler>();
  const GameInstance::Send send = sendingThrough(*link_);
  games_.add(std::make_unique<GameInstance>(
      send,
      GameSettings{.seed = settings_.seed.value_or(std::random_device{}()),
                   .waves = std::move(waves)},
      *events_));
  GameInstance& game = games_.joinable();
  broadcaster_ =
      std::make_unique<GameEventBroadcaster>(GameEventBroadcaster::Wiring{
          .events = *events_,
          .timers = *timers_,
          .recipients = [&game] { return game.roster().endpoints(); },
          .headerFor =
              [&game](const network::Endpoint& recipient) {
                return game.nextSessionHeaderFor(recipient);
              },
          .send = send});
}

void GamesModule::close() {
  broadcaster_.reset();
  games_.clear();
  timers_.reset();
  events_.reset();
}

void GamesModule::playTick() {
  for (const std::unique_ptr<GameInstance>& game : games_.all()) {
    game->onTick(received_->players);
    forgetDepartedPlayers(*game, *link_);
  }
}

void GamesModule::deliverEvents(engine::Duration elapsed) {
  timers_->advance(elapsed);
  events_->dispatch();
}

void GamesModule::sayGoodbye() {
  try {
    sendDisconnections(games_, *link_);
    for (std::size_t copy = 1; copy < REPEATED_MESSAGE_COPIES; ++copy) {
      std::this_thread::sleep_for(REPEATED_MESSAGE_INTERVAL);
      sendDisconnections(games_, *link_);
    }
  } catch (const std::exception& error) {
    logger_.warn("the players were not all told the server stops: ",
                 error.what());
  }
}

}  // namespace rtype::server
