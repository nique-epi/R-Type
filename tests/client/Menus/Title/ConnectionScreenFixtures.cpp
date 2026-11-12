#include "ConnectionScreenFixtures.hpp"
#include <string>
#include <vector>
#include "ConnectionPhase.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "RecordingServerConnection.hpp"
#include "ServerChoice.hpp"
#include "ServerConnectionRequest.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::ConnectionStatus;
using rtype::client::ServerChoice;
using rtype::client::testing::LAUNCH_ADDRESS;
using rtype::client::testing::LAUNCH_GAME;
using rtype::client::testing::LAUNCH_NICKNAME;
using rtype::client::testing::LAUNCH_PORT;
using rtype::client::testing::NEXT_MAP_ADDRESS;
using rtype::client::testing::NEXT_MAP_PORT;
using rtype::network::Endpoint;

std::vector<ServerConnectionCall> connectAttemptsOf(
    const RecordingServerConnection& connection) {
  std::vector<ServerConnectionCall> connects;
  for (const ServerConnectionCall& call : connection.calls()) {
    if (call.request == ServerConnectionRequest::Connect) {
      connects.push_back(call);
    }
  }
  return connects;
}

ServerChoice launchChoice() {
  return ServerChoice{.server = Endpoint{.address = std::string(LAUNCH_ADDRESS),
                                         .port = LAUNCH_PORT},
                      .nickname = std::string(LAUNCH_NICKNAME),
                      .game = LAUNCH_GAME,
                      .nextServers = {}};
}

Endpoint nextMapServer() {
  return Endpoint{.address = std::string(NEXT_MAP_ADDRESS),
                  .port = NEXT_MAP_PORT};
}

ConnectionStatus refusedFor(rtype::network::RefusalReason reason) {
  return ConnectionStatus{.phase = ConnectionPhase::Refused, .refusal = reason};
}
