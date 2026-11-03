#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IServerConnection.hpp"

/** @brief What a screen asked of the connection: to start an attempt, with its
 * server and its nickname, or to drop it. */
struct ServerConnectionCall {
  enum class Request : std::uint8_t {
    Connect,
    Cancel,
  };

  Request request{};
  rtype::network::Endpoint server;
  std::string nickname;

  bool operator==(const ServerConnectionCall& other) const = default;
};

/** @brief Prints a request in test failure messages. */
void PrintTo(const ServerConnectionCall& call, std::ostream* output);

/**
 * @brief Connection double that writes down every request, in order, and whose
 * status the test sets: a real connection changes it on its own, at any time
 * between two updates.
 */
class RecordingServerConnection : public rtype::client::IServerConnection {
 public:
  /** @brief Writes the request down; the status stays what the test set. */
  void connect(const rtype::network::Endpoint& server,
               std::string_view nickname) override;
  /** @brief Writes the request down, and goes back to no attempt and no
   * session, as a real connection does. */
  void cancel() override;
  [[nodiscard]] rtype::client::ConnectionStatus status() const override;

  void setStatus(rtype::client::ConnectionStatus status);
  [[nodiscard]] const std::vector<ServerConnectionCall>& calls() const;

 private:
  rtype::client::ConnectionStatus status_;
  std::vector<ServerConnectionCall> calls_;
};
