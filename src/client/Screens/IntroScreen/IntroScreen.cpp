#include "IntroScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <ratio>
#include <span>
#include <string>
#include <utility>
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "IMusicClock.hpp"
#include "IMusicPlayer.hpp"
#include "IntroCastBuilder.hpp"
#include "IntroCue.hpp"
#include "IntroCues.hpp"
#include "IntroFrame.hpp"
#include "IntroFreeze.hpp"
#include "IntroOverlay.hpp"
#include "IntroOverlayConstants.hpp"
#include "IntroOverlayLook.hpp"
#include "IntroPainting.hpp"
#include "IntroScreenParts.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "Logger.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayerPreferences.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The freezes the scene goes through: none with REDUCE FLASHES. */
std::span<const IntroFreeze> freezesFor(FlashPreference preference) {
  return preference == FlashPreference::Full
             ? std::span<const IntroFreeze>(INTRO_FREEZES)
             : std::span<const IntroFreeze>();
}

/** @returns The logo of the title, its whole texture. */
TextureArea titleLogo() {
  return TextureArea{.assetId = LOGO_ID,
                     .rectangle = sf::IntRect({0, 0}, LOGO_SIZE)};
}

/** @returns Where the music is, in seconds; nothing while it does not play. */
std::optional<float> musicSecondsOf(const IMusicClock& music) {
  const std::optional<engine::Duration> offset = music.playingOffset();
  if (!offset.has_value()) {
    return std::nullopt;
  }
  return secondsOf(*offset);
}

/** @brief Writes how far the music was ahead of the intro when it gave way to
 * the title: how well the intro followed it on this machine. */
void logGiveWay(const logging::Logger& logger,
                std::optional<engine::Duration> gap) {
  if (!gap.has_value()) {
    logger.info("the intro gives way to the title, without music");
    return;
  }
  logger.info("the intro gives way to the title, the music ",
              std::chrono::duration<float, std::milli>(*gap).count(),
              " ms ahead of it");
}

}  // namespace

IntroScreen::IntroScreen(IntroScreenParts parts,
                         std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      canvas_(std::move(canvas)),
      sky_(&parts.sky.get()),
      sounds_(&parts.sounds.get()),
      music_(&parts.music.get()),
      logger_(std::string(INTRO_LOG_MODULE_NAME)),
      preference_(flashPreferenceOf(parts.settings.get())),
      palette_(playerPaletteOf(parts.settings.get())),
      freezes_(freezesFor(preference_)),
      clock_(parts.music.get()),
      cast_(buildIntroCast(parts.seed)),
      cues_(introCues(cast_, freezes_)) {
  clock_.jumpTo(durationOf(-INTRO_BLACK_TIME));
  moveSky(-INTRO_BLACK_TIME);
  repaint(-INTRO_BLACK_TIME, -INTRO_BLACK_TIME);
}

IntroScreen::~IntroScreen() {
  sky_->setSpeedFactor(1.0F);
  sky_->setOffset({});
}

void IntroScreen::handleEvent(const sf::Event& event) {
  if (event.is<sf::Event::KeyPressed>() ||
      event.is<sf::Event::MouseButtonPressed>()) {
    skip();
  }
}

void IntroScreen::update(engine::Duration elapsed) {
  clock_.advance(elapsed);
  const float real = secondsOf(clock_.now());
  if (real >= INTRO_TITLE) {
    if (!titleAsked_) {
      logGiveWay(logger_, clock_.gapToMusic());
      askForTheTitle(TITLE_AFTER_INTRO_SCREEN);
    }
    return;
  }
  if (real >= INTRO_MUSIC_START) {
    music_->start();
  }
  const float scene = sceneTimeAt(real, freezes_);
  playCuesUpTo(scene);
  shake_.advance(elapsed);
  moveSky(scene);
  repaint(real, scene);
}

void IntroScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool IntroScreen::hidesScreensBelow() const { return true; }

void IntroScreen::skip() {
  const float now = secondsOf(clock_.now());
  if (now < 0.0F) {
    askForTheTitle(TITLE_SCREEN);
    return;
  }
  if (skipped_ || now >= INTRO_SLAM) {
    askForTheTitle(TITLE_AFTER_INTRO_SCREEN);
    return;
  }
  skipped_ = true;
  const float target = skipTargetAt(now, musicSecondsOf(*music_));
  clock_.jumpTo(durationOf(target));
  const auto jumpedOver = std::ranges::upper_bound(
      cues_, sceneTimeAt(target, freezes_), {}, &IntroCue::at);
  nextCue_ =
      std::max(nextCue_, static_cast<std::size_t>(jumpedOver - cues_.begin()));
}

void IntroScreen::askForTheTitle(ScreenId title) {
  if (titleAsked_) {
    return;
  }
  titleAsked_ = true;
  navigator_->replaceAll(title);
}

void IntroScreen::playCuesUpTo(float scene) {
  for (; nextCue_ < cues_.size() && cues_[nextCue_].at <= scene; ++nextCue_) {
    const IntroCue& cue = cues_[nextCue_];
    if (!cue.sound.empty()) {
      sounds_->play(cue.sound);
    }
    if (preference_ == FlashPreference::Full) {
      shake_.addTrauma(cue.trauma);
    }
  }
}

void IntroScreen::moveSky(float scene) {
  sky_->setSpeedFactor(skyFactorAt(scene));
  sky_->setOffset(shake_.offset());
}

void IntroScreen::repaint(float real, float scene) {
  canvas_->clear();
  paintIntroCast(*canvas_, IntroFrame{.cast = cast_,
                                      .real = real,
                                      .scene = scene,
                                      .view = cinemaViewAt(real),
                                      .shake = shake_.offset(),
                                      .drift = debrisDriftAt(scene),
                                      .preference = preference_,
                                      .palette = palette_,
                                      .squadronBox = INTRO_SQUADRON_BOX_PLACE});
  paintIntroOverlay(*canvas_, real,
                    IntroOverlayLook{.logo = titleLogo(), .palette = palette_});
  canvas_->display();
}

}  // namespace rtype::client
