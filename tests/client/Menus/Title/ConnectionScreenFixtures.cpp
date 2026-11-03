#include "ConnectionScreenFixtures.hpp"
#include <string>
#include <vector>
#include "ConnectionScreenTestConstants.hpp"
#include "Endpoint.hpp"
#include "RecordingServerConnection.hpp"
#include "ServerChoice.hpp"

using rtype::client::ServerChoice;
using rtype::client::testing::LAUNCH_ADDRESS;
using rtype::client::testing::LAUNCH_NICKNAME;
using rtype::client::testing::LAUNCH_PORT;
using rtype::network::Endpoint;

std::vector<ServerConnectionCall> connectAttemptsOf(
    const RecordingServerConnection& connection) {
  std::vector<ServerConnectionCall> connects;
  for (const ServerConnectionCall& call : connection.calls()) {
    if (call.request == ServerConnectionCall::Request::Connect) {
      connects.push_back(call);
    }
  }
  return connects;
}

ServerChoice launchChoice() {
  return ServerChoice{.server = Endpoint{.address = std::string(LAUNCH_ADDRESS),
                                         .port = LAUNCH_PORT},
                      .nickname = std::string(LAUNCH_NICKNAME)};
}
