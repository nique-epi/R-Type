#include "ConnectionScreenFixtures.hpp"
#include <vector>
#include "RecordingServerConnection.hpp"

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
