#pragma once

#include <functional>
#include <string>
#include "Endpoint.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/** @brief What a connection screen is built from, but the picture it owns. */
struct ConnectionScreenParts {
  /** @brief Screens the connection screen lives among, which must outlive
   * it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Connection the attempts go through, which must outlive the
   * screen. */
  std::reference_wrapper<IServerConnection> connection;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Address and port the fields start with. */
  network::Endpoint server;
  /** @brief Nickname the field starts with. */
  std::string nickname;
};

}  // namespace rtype::client
