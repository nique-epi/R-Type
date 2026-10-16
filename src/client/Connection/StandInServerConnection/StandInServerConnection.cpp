#include "StandInServerConnection.hpp"
#include <string>
#include <string_view>
#include "ConnectionConstants.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "Logger.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

void StandInServerConnection::connect(
    const network::Endpoint& server,
    [[maybe_unused]] std::string_view nickname) {
  const logging::Logger logger{std::string{CONNECTION_LOGGER_NAME}};
  logger.warn("stand-in connection: ", server.address, " port ", server.port,
              " not contacted, accepted at once as player ",
              static_cast<unsigned int>(network::FIRST_PLAYER_NUMBER));
  status_ = {.phase = ConnectionStatus::Phase::Accepted,
             .playerNumber = network::FIRST_PLAYER_NUMBER};
}

void StandInServerConnection::cancel() { status_ = {}; }

ConnectionStatus StandInServerConnection::status() const { return status_; }

}  // namespace rtype::client
