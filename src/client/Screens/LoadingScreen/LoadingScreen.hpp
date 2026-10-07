#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "IScreen.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

/** @brief Loads the assets of a game one file per frame while showing the
 * progress, then replaces every screen with the next one. */
class LoadingScreen : public IScreen {
 public:
  /** @brief Releases the loaded assets the list does not name, except the
   * permanent ones. */
  LoadingScreen(ScreenStack& screens, AssetLibrary& library,
                const AssetList& assets, ScreenFactory nextScreen);

  /** @brief Ignores every event. */
  void handleEvent(const sf::Event& event) override;

  /** @brief Loads the next file, then asks for the next screen once none is
   * left. */
  void update() override;

  void draw(sf::RenderTarget& target) const override;

 private:
  void showProgress();

  ScreenStack* screens_;
  AssetLoadingStep loading_;
  ScreenFactory nextScreen_;
  sf::RectangleShape barOutline_;
  sf::RectangleShape barFill_;
};

}  // namespace rtype::client
