#pragma once

#include <functional>
#include "AssetLibrary.hpp"
#include "ChatChannel.hpp"
#include "EntityMessageQueue.hpp"
#include "GameWindow.hpp"
#include "IAudioService.hpp"
#include "IEntityTypeSource.hpp"
#include "IEntityTypeTable.hpp"
#include "IGameCatalog.hpp"
#include "IServerCatalog.hpp"
#include "IServerTicks.hpp"
#include "PlayerSettings.hpp"
#include "ServerChoice.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerSession.hpp"
#include "SettingsService.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief What the screens of the client are built from: the services of the
 * modules listed before the screens module (the sound included), and the
 * server, the nickname and the game the first connection screen starts with.
 * Every reference must outlive the screens.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ScreenServices {
  std::reference_wrapper<PlayerSettings> settings;
  std::reference_wrapper<SettingsService> settingsService;
  std::reference_wrapper<AssetLibrary> assets;
  /** @brief The content of the session: a game loads the sprite sheets of
   * its appearances. */
  std::reference_wrapper<const IEntityTypeTable> entityTable;
  std::reference_wrapper<IAudioService> audio;
  std::reference_wrapper<GameWindow> window;
  std::reference_wrapper<ServerSession> connection;
  std::reference_wrapper<WaitingRoom> waitingRoom;
  std::reference_wrapper<EntityMessageQueue> entityMessages;
  std::reference_wrapper<ServerLeaderboard> leaderboard;
  /** @brief The chat of the session, shown by the waiting room and the
   * game. */
  std::reference_wrapper<ChatChannel> chat;
  /** @brief What the entity type numbers of the server mean, for the
   * replicator of a game. */
  std::reference_wrapper<const IEntityTypeSource> entityTypes;
  std::reference_wrapper<const IServerTicks> serverTicks;
  /** @brief The servers the master lists, with their ping, and the games of
   * one of them, for the server map. */
  std::reference_wrapper<IServerCatalog> serverCatalog;
  std::reference_wrapper<IGameCatalog> gameCatalog;
  /** @brief The server, the nickname and the game the first connection
   * screen starts with. */
  ServerChoice choice;
};

}  // namespace rtype::client
