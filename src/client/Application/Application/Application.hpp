#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include "AssetList.hpp"
#include "Endpoint.hpp"
#include "GameResult.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "MusicPlayer.hpp"
#include "NetworkThread.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "ScrollingBackground.hpp"
#include "ServerChoice.hpp"
#include "ServerConnection.hpp"
#include "SettingsFile.hpp"
#include "SoundPlayer.hpp"
#include "Soundtrack.hpp"
#include "SystemClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief The client once its interface assets are loaded: opens the window
 * and runs the frame loop over the screens.
 *
 * Each frame, the window events go to the screen on top, the connection to a
 * server reads what arrived and sends what is due, the scrolling background
 * and every screen are updated, then the background is drawn and
 * every screen over it, from the bottom up. The changes the screens ask for are
 * applied after each event and after the update, so the next event already
 * reaches the new screen on top.
 *
 * The background and every screen move by the real time elapsed since the
 * previous frame, so their speed does not depend on the frame rate. It is drawn
 * under every screen for now, the loading screen and the options included.
 *
 * The loop never waits on the network: the socket runs on a NetworkThread,
 * opened on every IPv4 interface on a port the system picks, and hands its
 * datagrams over through a queue the connection drains each frame.
 *
 * It owns the connection to a server, which outlives every screen: the
 * connection screen drives it, and the game sends its inputs through it and
 * takes the entities of the server from it. It owns the player's settings too,
 * read from the settings file at launch: the window opens at the size they
 * hold, the game reads their keys, the options change and save them.
 *
 * It owns the sounds every screen plays, on SOUND_VOICE_LIMIT voices for the
 * whole client, and the music: the theme, started by the intro after its
 * black second or by a title screen, and the music of a game, which a game
 * starts a silence after stopping the theme (Soundtrack); their volume
 * follows the settings at each frame.
 *
 * When the server ends a game, the game screen writes its result here and
 * gives way to the end screen, from which the player plays again in the
 * waiting room or quits to the title.
 *
 * Screens are asked for by identifier; createScreen() is the one place that
 * builds them, with one case per ScreenId. The client opens on the intro, or
 * on the title screen when the system asks for less motion.
 */
class Application {
 public:
  /**
   * @param assets Library holding the interface assets, loaded and permanent;
   * it must outlive the application.
   * @param server Address and port the first connection screen starts with.
   * @param nickname Nickname the first connection screen starts with.
   * @param settingsFile File the settings are read from and saved to, or
   * nothing to keep them for this launch only.
   * @throws RenderTextureNotCreatedException when the background cannot get
   * its render textures.
   * @throws SocketOpenException when the system refuses to open the socket.
   */
  Application(AssetLibrary& assets, network::Endpoint server,
              std::string nickname,
              std::optional<std::filesystem::path> settingsFile);

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;
  ~Application() = default;

  /**
   * @brief Runs the frame loop until the window is closed or no screen is
   * left, then leaves the session with the server, if any: it returns once
   * every copy of the Disconnection went out, DISCONNECTION_COPY_INTERVAL
   * apart, so the server frees the place of the player at once.
   *
   * @throws AssetLoadingException when an asset of the game cannot be loaded.
   * @throws RenderTextureNotCreatedException when the background cannot be
   * enlarged to the size of the window.
   */
  void run();

 private:
  void handleEvents();

  /** @brief Leaves the session with the server, if any, and waits until the
   * connection sent every copy of the Disconnection. */
  void leaveServer();

  /** @returns The real time since the previous frame, which the background
   * and every screen move by. */
  [[nodiscard]] engine::Duration measureFrame();

  void render();

  /** @brief Builds the screen of an identifier: the list of every screen of
   * the client. */
  [[nodiscard]] std::unique_ptr<IScreen> createScreen(ScreenId screen);

  /** @brief The loading screen of titleAssets() while some are not loaded,
   * the title screen once they all are; its squadron is still when the system
   * asks for less motion, and starts in place for ScreenId::TitleAfterIntro.
   * It plays the theme, unless it plays already, and stops the music of a
   * game: the intro starts the theme after its black second, and a client
   * without intro starts it with its title. */
  [[nodiscard]] std::unique_ptr<IScreen> titleScreen(ScreenId screen);

  /** @brief The loading screen of gameAssets() while some are not loaded, the
   * game screen once they all are. */
  [[nodiscard]] std::unique_ptr<IScreen> gameScreen();

  /** @brief The options, over the title for ScreenId::Options, over a game
   * for ScreenId::GameOptions. */
  [[nodiscard]] std::unique_ptr<IScreen> optionsScreen(ScreenId screen);

  /** @brief The waiting room, its READY key read from the bindings of the
   * player when it is built; for ScreenId::LobbyAfterGame, alone after a
   * game, playing the theme again, its BACK leading to the title. */
  [[nodiscard]] std::unique_ptr<IScreen> lobbyScreen(ScreenId screen);

  /**
   * @brief The loading screen of a list while some of its assets are not
   * loaded; nothing once they all are, after releasing the loaded assets the
   * list does not name, as the loading screen does.
   *
   * @param screen Screen the loading screen gives way to.
   */
  [[nodiscard]] std::unique_ptr<IScreen> loadingScreenUnlessLoaded(
      const AssetList& assets, ScreenId screen);

  /** @brief A new picture for a menu, written with the fonts of the asset
   * library. */
  [[nodiscard]] std::unique_ptr<IInterfaceCanvas> interfaceCanvas() const;

  /** @brief Gives the point of the playfield under a pixel of the window. */
  [[nodiscard]] PlayfieldMapping playfieldMapping() const;

  AssetLibrary* assets_;
  /**
   * @brief The settings file and what it held at launch, declared before the
   * window, which opens at the saved size, and before the screens, which read
   * and change them.
   */
  SettingsFile settingsFile_;
  PlayerSettings settings_;
  GameWindow window_;
  /** @brief The sky behind every screen, declared before the screens so it
   * outlives the game screen, which speeds it up and shakes it. */
  ScrollingBackground background_;
  /** @brief The sounds every screen plays and the music, declared after the
   * settings they read and before the screens that play them. */
  SoundPlayer sounds_;
  MusicPlayer theme_;
  MusicPlayer gameMusic_;
  Soundtrack soundtrack_;
  engine::SystemClock clock_;
  engine::Duration previousFrameTime_{};
  /**
   * @brief The socket and the connection the connection screen drives,
   * declared before the screens so they outlive them, the socket first since
   * the connection sends through it.
   */
  NetworkThread network_;
  ServerConnection connection_;
  /** @brief The server and the nickname every connection screen starts
   * with: those of launch, then those of the last attempt. */
  ServerChoice serverChoice_;
  /** @brief How the last game ended, written by the game screen, read by
   * the end screen. */
  GameResult gameResult_;
  /** @brief The taunt of the last end screen, never picked twice in a row. */
  std::optional<std::size_t> previousTaunt_;
  ScreenStack screens_;
};

}  // namespace rtype::client
