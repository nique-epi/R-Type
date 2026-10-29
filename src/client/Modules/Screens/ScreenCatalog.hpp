#pragma once

#include <memory>
#include "AssetList.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenCatalogParts.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief Builds the screen of an identifier: the list of every screen of the
 * client, one case per ScreenId, until modules register screens of their own.
 *
 * A screen whose assets are not all loaded is preceded by the loading screen
 * of those assets, which asks for it again once they are. A title screen
 * plays the theme, unless it plays already, and stops the music of a game; a
 * game plays the music of a game; the waiting room after a game plays the
 * theme again.
 */
class ScreenCatalog {
 public:
  explicit ScreenCatalog(ScreenCatalogParts parts);

  /** @returns The screen of @p screen, or its loading screen. */
  [[nodiscard]] std::unique_ptr<IScreen> create(ScreenId screen);

  /** @returns The screen the client opens on: the intro, or the title screen
   * when the system asks for less motion. */
  [[nodiscard]] static ScreenId firstScreen();

 private:
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
      const AssetList& assets, ScreenId screen) const;

  /** @brief A new picture for a menu, written with the fonts of the asset
   * library. */
  [[nodiscard]] std::unique_ptr<IInterfaceCanvas> interfaceCanvas() const;

  /** @brief Gives the point of the playfield under a pixel of the window. */
  [[nodiscard]] PlayfieldMapping playfieldMapping() const;

  ScreenCatalogParts parts_;
};

}  // namespace rtype::client
