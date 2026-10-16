#pragma once

#include <string_view>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "IServerConnection.hpp"

namespace rtype::client {

/**
 * @brief Provisional stand-in for the connection to a server: it talks to no
 * server, and accepts every attempt at once, as the first player.
 *
 * It keeps PLAY leading to the game while the client cannot join a server yet:
 * the connection screen shows "CONNECTED AS PLAYER 1" though no server took
 * part, and each attempt logs a warning that says so. Replace it, in the
 * constructor of Application, with the class that sends ConnectionRequest,
 * sends it again until an answer comes and reads that answer; nothing else
 * names it.
 */
class StandInServerConnection : public IServerConnection {
 public:
  /** @brief Accepts at once, as FIRST_PLAYER_NUMBER, whatever the server and
   * the nickname. */
  void connect(const network::Endpoint& server,
               std::string_view nickname) override;
  void cancel() override;
  [[nodiscard]] ConnectionStatus status() const override;

 private:
  ConnectionStatus status_;
};

}  // namespace rtype::client
