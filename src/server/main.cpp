#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <random>
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
    rtype::server::ServerApplication* serverPointer = nullptr;
    const auto queueDatagram =
        [&serverPointer](rtype::network::OutgoingDatagram datagram) {
          serverPointer->queueDatagram(std::move(datagram));
        };
    std::vector<std::unique_ptr<rtype::server::GameInstance>> games;
    const std::uint32_t seed =
        launchOptions.seed().value_or(std::random_device{}());
    games.push_back(std::make_unique<rtype::server::GameInstance>(
        queueDatagram, seed, events));
    rtype::server::GameInstance& joinableGame = *games.front();
    std::vector<rtype::network::Endpoint> eventRecipients;
    const auto gameEventBroadcaster =
        std::make_unique<rtype::server::GameEventBroadcaster>(
            rtype::server::GameEventBroadcaster::Wiring{
                .events = events,
                .timers = timers,
                .recipients = eventRecipients,
                .headerFor =
                    [&joinableGame](const rtype::network::Endpoint& recipient) {
                      return joinableGame.nextSessionHeaderFor(recipient);
                    },
                .send = queueDatagram});
    rtype::server::ConnectionAcceptor connectionAcceptor{
        joinableGame.roster(), queueDatagram,
        [&serverPointer](const rtype::network::Endpoint& player,
                         std::uint64_t sessionToken) {
          serverPointer->admit(player, sessionToken);
        },
        [&joinableGame,
         &eventRecipients](rtype::server::ConnectedPlayer& player) {
          joinableGame.onPlayerJoined(player);
          eventRecipients.push_back(player.endpoint);
        }};
    const rtype::engine::SystemClock clock;
    rtype::server::ServerApplication server{
        rtype::network::Endpoint{
            .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
            .port = launchOptions.port()},
        [&games, &joinableGame, &connectionAcceptor, &clock, &events, &timers,
         &serverPointer, &eventRecipients](
            const rtype::server::ServerApplication::ReceivedDatagrams&
                received) {
          for (const std::unique_ptr<rtype::server::GameInstance>& game :
               games) {
            game->onTick(received.players);
            for (const rtype::server::ConnectedPlayer& departed :
                 game->takeDepartedPlayers()) {
              serverPointer->forget(departed.endpoint, departed.sessionToken);
              std::erase(eventRecipients, departed.endpoint);
            }
          }
          connectionAcceptor.setGameInProgress(
              joinableGame.state() !=
              rtype::server::GameState::WaitingForPlayers);
          connectionAcceptor.receive(received.connectionRequests, clock.now());
          timers.advance(rtype::engine::SIMULATION_TICK_DURATION);
          events.dispatch();
        }};
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
