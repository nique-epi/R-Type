#pragma once

#include <functional>
#include <string>
#include "OutgoingDatagram.hpp"

namespace rtype::server {

/** @brief What the ServerInformationDesk calls for each answer to send. */
using InformationSend = std::function<void(network::OutgoingDatagram)>;

/** @brief What a ServerInformationDesk is built from. */
struct ServerInformationDeskParts {
  /** @brief The name it answers, a valid server name. */
  std::string name;
  /** @brief The game version it answers, a valid game version. */
  std::string gameVersion;
  /** @brief Must not be empty. */
  InformationSend send;
};

}  // namespace rtype::server
