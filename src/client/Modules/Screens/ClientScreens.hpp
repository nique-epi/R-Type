#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <optional>
#include "ClientScreensParts.hpp"
#include "GameResult.hpp"
#include "MusicPlayer.hpp"
#include "ScreenCatalog.hpp"
#include "ScreenStack.hpp"
#include "ScrollingBackground.hpp"
#include "SettingsService.hpp"
#include "SoundPlayer.hpp"
#include "Soundtrack.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The screens of the client and what they share: the sky drawn behind
 * every screen, the sounds every screen plays, on SOUND_VOICE_LIMIT voices for
 * the whole client, the music, and the result of the last game.
 *
 * The music is the theme, started by the intro after its black second or by a
 * title screen, and the music of a game, which a game starts a silence after
 * stopping the theme (Soundtrack); their volume follows the settings service,
 * which tells them each time it moves. When the server ends a game, the game
 * screen writes its result here and gives way to the end screen, from which the
 * player plays again in the waiting room or quits to the title.
 *
 * Each frame, the events go to the screen on top, the music, the sky and every
 * screen are updated, then the sky is drawn and every screen over it, from the
 * bottom up. The changes the screens ask for are applied after each event and
 * after the update, so the next event already reaches the new screen on top.
 * The sky and every screen move by the real time elapsed since the previous
 * frame, so their speed does not depend on the frame rate. The sky is drawn
 * under every screen for now, the loading screen and the options included.
 *
 * The screens keep pointers to it, so it can be neither copied nor moved.
 */
class ClientScreens {
 public:
  /**
   * @brief Builds the sky, the sounds, the music and the screens, then opens
   * on ScreenCatalog::firstScreen().
   * @throws RenderTextureNotCreatedException when the sky cannot get its
   * render textures.
   */
  explicit ClientScreens(const ClientScreensParts& parts);

  ClientScreens(const ClientScreens&) = delete;
  ClientScreens& operator=(const ClientScreens&) = delete;
  ClientScreens(ClientScreens&&) = delete;
  ClientScreens& operator=(ClientScreens&&) = delete;
  ~ClientScreens() = default;

  /** @brief Gives @p event to the screen on top, then applies the changes it
   * asked for. */
  void handleEvent(const sf::Event& event);

  /**
   * @brief Updates the music, the sky and every screen by @p elapsed, then
   * applies the changes the screens asked for.
   * @throws AssetLoadingException when an asset of the game cannot be loaded.
   */
  void update(engine::Duration elapsed);

  /**
   * @brief Draws the sky, then the screens over it, from the bottom up.
   * @throws RenderTextureNotCreatedException when the sky cannot be enlarged
   * to the size of the window.
   */
  void draw(sf::RenderTarget& target);

  [[nodiscard]] bool isEmpty() const;

  /** @brief Stops the sounds and the music from hearing the settings service:
   * called before the service goes away. */
  void stopListening();

 private:
  SettingsService* settingsService_;
  ScrollingBackground background_;
  SoundPlayer sounds_;
  MusicPlayer theme_;
  MusicPlayer gameMusic_;
  Soundtrack soundtrack_;
  GameResult gameResult_;
  std::optional<std::size_t> previousTaunt_;
  ScreenStack screens_;
  ScreenCatalog catalog_;
};

}  // namespace rtype::client
