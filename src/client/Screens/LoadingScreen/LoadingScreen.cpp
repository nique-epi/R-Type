#include "LoadingScreen.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "AssetLoadingStep.hpp"
#include "IScreenNavigator.hpp"
#include "LoadingScreenParts.hpp"
#include "PlayfieldConstants.hpp"
#include "ScreenColors.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

sf::Vector2f centerOf(sf::Vector2f size) {
  return sf::FloatRect({0.0F, 0.0F}, size).getCenter();
}

float loadedFraction(const AssetLoadingStep& loading) {
  if (loading.assetCount() == 0) {
    return 1.0F;
  }
  return static_cast<float>(loading.processedCount()) /
         static_cast<float>(loading.assetCount());
}

}  // namespace

LoadingScreen::LoadingScreen(const LoadingScreenParts& parts)
    : navigator_(&parts.navigator.get()),
      loading_(parts.library.get(), parts.assets.get()),
      nextScreen_(parts.nextScreen),
      barOutline_({LOADING_BAR_WIDTH, LOADING_BAR_HEIGHT}) {
  const sf::Vector2f barTopLeft =
      centerOf({game::PLAYFIELD_WIDTH, game::PLAYFIELD_HEIGHT}) -
      centerOf(barOutline_.getSize());
  barOutline_.setPosition(barTopLeft);
  barOutline_.setFillColor(sf::Color::Transparent);
  barOutline_.setOutlineColor(LOADING_BAR_OUTLINE_COLOR);
  barOutline_.setOutlineThickness(LOADING_BAR_OUTLINE_THICKNESS);
  barFill_.setPosition(barTopLeft);
  barFill_.setFillColor(LOADING_BAR_FILL_COLOR);
  showProgress();
}

void LoadingScreen::handleEvent([[maybe_unused]] const sf::Event& event) {}

void LoadingScreen::update([[maybe_unused]] engine::Duration elapsed) {
  if (!loading_.isFinished()) {
    loading_.loadNext();
    showProgress();
  }
  if (loading_.isFinished() && !nextScreenAsked_) {
    navigator_->replaceAll(nextScreen_);
    nextScreenAsked_ = true;
  }
}

void LoadingScreen::draw(sf::RenderTarget& target) const {
  target.draw(barOutline_);
  target.draw(barFill_);
}

bool LoadingScreen::hidesScreensBelow() const { return true; }

void LoadingScreen::showProgress() {
  barFill_.setSize(
      {LOADING_BAR_WIDTH * loadedFraction(loading_), LOADING_BAR_HEIGHT});
}

}  // namespace rtype::client
