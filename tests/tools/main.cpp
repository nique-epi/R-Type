#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>
#include "DatagramFlood.hpp"
#include "FloodConstants.hpp"
#include "FloodLaunch.hpp"
#include "FloodLaunchOptions.hpp"
#include "FloodMode.hpp"
#include "FloodRecipient.hpp"
#include "FloodReport.hpp"
#include "LaunchArguments.hpp"
#include "LoopbackPeer.hpp"
#include "MalformedDatagramConstants.hpp"
#include "NetworkConstants.hpp"
#include "SessionOpening.hpp"

namespace {

/**
 * @brief Prints how many datagrams of each kind @p report sent, how many the
 * system refused, and how many the recipient sent meanwhile.
 */
void printReport(const rtype::tools::FloodReport& report) {
  std::cout << "sent in " << rtype::tools::FLOOD_DURATION.count() << " s:\n";
  for (std::size_t kind = 0;
       kind < rtype::tools::MALFORMED_DATAGRAM_KINDS.size(); ++kind) {
    std::cout << "  " << rtype::tools::MALFORMED_DATAGRAM_KIND_NAMES.at(kind)
              << ": " << report.sentByKind.at(kind) << '\n';
  }
  std::cout << "refused by the system: " << report.refusedBySystemCount << '\n'
            << "received from the recipient meanwhile: " << report.receivedCount
            << '\n';
}

/**
 * @returns The exit code of a flood of the server listening on
 * @p serverPort: 0 when it accepted the tool and still answers a connection
 * request after the flood, 1 otherwise.
 */
int floodServer(std::uint16_t serverPort) {
  rtype::tools::LoopbackPeer peer{rtype::network::ANY_PORT};
  const std::optional<std::uint64_t> sessionToken =
      rtype::tools::joinServer(peer, serverPort);
  if (!sessionToken.has_value()) {
    std::cerr << "the server on port " << serverPort
              << " did not accept the tool as a player\n";
    return 1;
  }
  rtype::tools::DatagramFlood flood{
      rtype::tools::FloodRecipient{.port = serverPort,
                                   .sessionToken = *sessionToken},
      rtype::tools::FLOOD_SEED};
  printReport(flood.run(peer));
  if (!rtype::tools::serverAnswers(serverPort)) {
    std::cout << "the server no longer answers a connection request\n";
    return 1;
  }
  std::cout << "the server still answers a connection request\n";
  return 0;
}

/**
 * @returns The exit code of a flood of the first client that connects to
 * @p listeningPort: 0 once the flood is over, 1 when no client came.
 */
int floodClient(std::uint16_t listeningPort) {
  rtype::tools::LoopbackPeer peer{listeningPort};
  std::cout << "waiting on port " << listeningPort << " for a client\n";
  const std::optional<std::uint16_t> clientPort =
      rtype::tools::acceptClient(peer);
  if (!clientPort.has_value()) {
    std::cerr << "no client asked to connect within "
              << rtype::tools::CLIENT_WAIT_TIMEOUT.count() << " s\n";
    return 1;
  }
  rtype::tools::DatagramFlood flood{
      rtype::tools::FloodRecipient{
          .port = *clientPort,
          .sessionToken = rtype::tools::TOOL_SESSION_TOKEN},
      rtype::tools::FLOOD_SEED};
  printReport(flood.run(peer));
  return 0;
}

/**
 * @returns The exit code of the flood of @p mode on @p port. An error stops
 * it and is printed: no exception leaves it.
 */
int runFlood(rtype::tools::FloodMode mode, std::uint16_t port) {
  try {
    return mode == rtype::tools::FloodMode::Server ? floodServer(port)
                                                   : floodClient(port);
  } catch (const std::exception& error) {
    std::cerr << rtype::tools::TOOL_PROGRAM_NAME << " stopped: " << error.what()
              << '\n';
    return 1;
  }
}

}  // namespace

int main(int argumentCount, char** arguments) {
  try {
    std::vector<std::string_view> launchArguments;
    for (int index = 1; index < argumentCount; ++index) {
      launchArguments.emplace_back(arguments[index]);
    }
    const rtype::tools::FloodLaunch launch =
        rtype::tools::readFloodLaunch(launchArguments);
    if (launch.helpRequested) {
      std::cout << rtype::tools::floodUsage();
      return 0;
    }
    if (!launch.mode.has_value()) {
      std::cerr << rtype::tools::floodUsage();
      return 1;
    }
    return runFlood(*launch.mode, launch.port);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(rtype::tools::TOOL_PROGRAM_NAME,
                                                error.what());
    return 1;
  }
}
