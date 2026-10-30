#pragma once

#include <functional>
#include <string>
#include "AssetLibrary.hpp"
#include "Endpoint.hpp"
#include "EntityMessageQueue.hpp"
#include "GameWindow.hpp"
#include "IAudioService.hpp"
#include "ISettingsStorage.hpp"
#include "PlayerSettings.hpp"
#include "ServerSession.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief What the screens of the client are built from: the services of the
 * modules listed before the screens module (the sound included), and the server
 * and the nickname the connection screen starts with. Every reference must
 * outlive the screens.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ClientScreensParts {
  std::reference_wrapper<PlayerSettings> settings;
  std::reference_wrapper<ISettingsStorage> storage;
  std::reference_wrapper<AssetLibrary> assets;
  std::reference_wrapper<IAudioService> audio;
  std::reference_wrapper<GameWindow> window;
  std::reference_wrapper<ServerSession> connection;
  std::reference_wrapper<WaitingRoom> waitingRoom;
  std::reference_wrapper<EntityMessageQueue> entityMessages;
  network::Endpoint server;
  std::string nickname;
};

}  // namespace rtype::client
