#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "SystemClock.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"

namespace {

using Games = std::vector<std::unique_ptr<rtype::server::GameInstance>>;

/**
 * @brief What each tick of the server works on. Everything must outlive the
 * server; the first game of the list is the one new players join.
 */
struct TickParts {
  std::reference_wrapper<Games> games;
  std::reference_wrapper<rtype::server::ConnectionAcceptor> acceptor;
  std::reference_wrapper<rtype::engine::EventBus> events;
  std::reference_wrapper<rtype::engine::TimerScheduler> timers;
  std::reference_wrapper<const rtype::engine::SystemClock> clock;
};

/**
 * @returns The address and port the server listens on: every IPv4 interface,
 * on the port of @p launchOptions.
 */
rtype::network::Endpoint listeningEndpoint(
    const rtype::server::LaunchOptions& launchOptions) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
      .port = launchOptions.port()};
}

/**
 * @returns What the broadcaster of the game events is built from: the bus and
 * the timers of @p parts, and as recipients the players of @p game, each under
 * its own session header; @p send puts a datagram in the outgoing queue.
 */
rtype::server::GameEventBroadcaster::Wiring eventBroadcasterWiring(
    const TickParts& parts, rtype::server::GameInstance& game,
    const rtype::server::GameInstance::Send& send) {
  return rtype::server::GameEventBroadcaster::Wiring{
      .events = parts.events,
      .timers = parts.timers,
      .recipients = [&game] { return game.roster().endpoints(); },
      .headerFor =
          [&game](const rtype::network::Endpoint& recipient) {
            return game.nextSessionHeaderFor(recipient);
          },
      .send = send};
}

/**
 * @brief Makes @p server forget the address of every player of @p game whose
 * session closed in this tick.
 */
void forgetDepartedPlayers(rtype::server::GameInstance& game,
                           rtype::server::ServerApplication& server) {
  for (const rtype::server::ConnectedPlayer& departed :
       game.takeDepartedPlayers()) {
    server.forget(departed.endpoint, departed.sessionToken);
  }
}

/**
 * @brief Answers @p requests, refused with GameInProgress unless the game new
 * players join waits for players.
 */
void answerConnectionRequests(
    const TickParts& parts,
    std::span<const rtype::network::IncomingDatagram> requests) {
  const rtype::server::GameInstance& joinableGame = *parts.games.get().front();
  rtype::server::ConnectionAcceptor& acceptor = parts.acceptor.get();
  acceptor.setGameInProgress(joinableGame.state() !=
                             rtype::server::GameState::WaitingForPlayers);
  acceptor.receive(requests, parts.clock.get().now());
}

/**
 * @brief Runs one tick of the server: each game reads the datagrams of its
 * players and moves on, the players whose session closed are forgotten, the
 * connection requests are answered, then the timers advance and the bus
 * delivers the events of the tick.
 */
void runTick(
    const TickParts& parts, rtype::server::ServerApplication& server,
    const rtype::server::ServerApplication::ReceivedDatagrams& received) {
  for (const std::unique_ptr<rtype::server::GameInstance>& game :
       parts.games.get()) {
    game->onTick(received.players);
    forgetDepartedPlayers(*game, server);
  }
  answerConnectionRequests(parts, received.connectionRequests);
  parts.timers.get().advance(rtype::engine::SIMULATION_TICK_DURATION);
  parts.events.get().dispatch();
}

/**
 * @brief Applies the logging options, then runs the server on the port of
 * @p launchOptions until it stops.
 * @returns the exit code: 0 once the server stopped, 1 after an error, which
 *          is logged and printed. No exception leaves it.
 */
int runServer(const rtype::logging::LogLaunchOptions& logOptions,
              const rtype::server::LaunchOptions& launchOptions) {
  try {
    logOptions.apply();
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
    logger.info("server starting");
    rtype::engine::EventBus events;
    rtype::engine::TimerScheduler timers;
    const rtype::engine::SystemClock clock;
    rtype::server::ServerApplication* serverPointer = nullptr;
    const rtype::server::GameInstance::Send queueDatagram =
        [&serverPointer](rtype::network::OutgoingDatagram datagram) {
          serverPointer->queueDatagram(std::move(datagram));
        };
    Games games;
    games.push_back(std::make_unique<rtype::server::GameInstance>(
        queueDatagram, launchOptions.seed().value_or(std::random_device{}()),
        events));
    rtype::server::GameInstance& joinableGame = *games.front();
    rtype::server::ConnectionAcceptor connectionAcceptor{
        joinableGame.roster(), queueDatagram,
        [&serverPointer](const rtype::network::Endpoint& player,
                         std::uint64_t sessionToken) {
          serverPointer->admit(player, sessionToken);
        },
        [&joinableGame](rtype::server::ConnectedPlayer& player) {
          joinableGame.onPlayerJoined(player);
        }};
    const TickParts parts{.games = games,
                          .acceptor = connectionAcceptor,
                          .events = events,
                          .timers = timers,
                          .clock = clock};
    const auto gameEventBroadcaster =
        std::make_unique<rtype::server::GameEventBroadcaster>(
            eventBroadcasterWiring(parts, joinableGame, queueDatagram));
    rtype::server::ServerApplication server{
        listeningEndpoint(launchOptions),
        [&parts, &serverPointer](
            const rtype::server::ServerApplication::ReceivedDatagrams&
                received) { runTick(parts, *serverPointer, received); }};
    serverPointer = &server;
    logger.info("listening on UDP port ", server.localEndpoint().port);
    server.run();
    logger.info("server stopped");
  } catch (const std::exception& error) {
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
    logger.error("server stopped: ", error.what());
    std::cerr << "server stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}

}  // namespace

int main(int argumentCount, char** arguments) {
  try {
    const rtype::logging::LogLaunchOptions logOptions =
        rtype::logging::LogLaunchOptions::readFromProcess(
            argumentCount, arguments, rtype::logging::SERVER_LOG_FILE_NAME);
    const rtype::server::LaunchOptions launchOptions =
        rtype::server::LaunchOptions::read(logOptions.otherArguments());
    if (launchOptions.helpRequested()) {
      std::cout << rtype::server::LaunchOptions::usage();
      return 0;
    }
    return runServer(logOptions, launchOptions);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(rtype::server::PROGRAM_NAME,
                                                error.what());
    return 1;
  }
}
