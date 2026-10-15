#include "RecordingServerConnection.hpp"
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"

void PrintTo(const ServerConnectionCall& call, std::ostream* output) {
  *output << "request " << static_cast<int>(call.request);
  if (call.request == ServerConnectionCall::Request::Connect) {
    *output << " to " << call.server.address << " port " << call.server.port
            << " as " << call.nickname;
  }
}

void RecordingServerConnection::connect(const rtype::network::Endpoint& server,
                                        std::string_view nickname) {
  calls_.push_back({.request = ServerConnectionCall::Request::Connect,
                    .server = server,
                    .nickname = std::string(nickname)});
}

void RecordingServerConnection::cancel() {
  calls_.push_back({.request = ServerConnectionCall::Request::Cancel,
                    .server = {},
                    .nickname = {}});
  status_ = {};
}

rtype::client::ConnectionStatus RecordingServerConnection::status() const {
  return status_;
}

void RecordingServerConnection::setStatus(
    rtype::client::ConnectionStatus status) {
  status_ = status;
}

const std::vector<ServerConnectionCall>& RecordingServerConnection::calls()
    const {
  return calls_;
}
