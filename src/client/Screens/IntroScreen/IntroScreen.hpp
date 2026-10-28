#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <span>
#include <vector>
#include "CameraShake.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "IMusicPlayer.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISky.hpp"
#include "ISoundPlayer.hpp"
#include "IntroCast.hpp"
#include "IntroCue.hpp"
#include "IntroFreeze.hpp"
#include "IntroScreenParts.hpp"
#include "Logger.hpp"
#include "PlayerPalette.hpp"
#include "ScreenId.hpp"
#include "SyncedClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The intro the client opens on: a second of black, then 5.5 s of
 * trailer on the beats of the theme (IntroTimelineConstants.hpp), then the
 * title screen, on the same sky.
 *
 * Its time starts at -INTRO_BLACK_TIME: the picture is black until 0, then
 * fades in. It starts the music itself at INTRO_MUSIC_START, so the first
 * beat of the theme lands on its time 0.
 *
 * Its time follows the position of the music while the music plays, and the
 * time of the frames otherwise (SyncedClock); the scene stops during the
 * freezes of INTRO_FREEZES, the music and the texts do not. Each update plays
 * the cues the scene reached (introCues()): their sounds, and their traumas on
 * the shake of the camera; then it sets the speed and the shake of the sky,
 * and paints the scene and the texts on its canvas, which draw() lays. At
 * INTRO_TITLE it writes how far the music was ahead of it, then asks for the
 * title in its place: ScreenId::TitleAfterIntro, whose squadron starts where
 * the ships of the intro landed. It is painted once when built, so its first
 * frame is already black.
 *
 * A key or a click during the black second opens the title at once, its
 * squadron flying in. Then the first one before INTRO_SLAM jumps to just
 * before the slam of the logo, so the logo slams on the next beat of the
 * music (skipTargetAt()), the cues jumped over left unplayed; the next one,
 * or one from the slam on, opens the title at once. A skip never moves the
 * music. With REDUCE FLASHES,
 * read when it is built, the scene does not freeze, the camera does not
 * shake, the bursts show their version without flash and no speed line is
 * drawn. The sky is put back at its speed and place when the intro goes.
 */
class IntroScreen : public IScreen {
 public:
  /** @param canvas Picture the intro is painted on, never null. */
  IntroScreen(IntroScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas);

  IntroScreen(const IntroScreen&) = delete;
  IntroScreen& operator=(const IntroScreen&) = delete;
  IntroScreen(IntroScreen&&) = delete;
  IntroScreen& operator=(IntroScreen&&) = delete;
  ~IntroScreen() override;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Jumps to just before the slam of the logo, so it slams on the next
   * beat of the music, the cues jumped over left unplayed; once skipped, or
   * from the slam on, asks for the title. */
  void skip();

  /** @brief Asks for that title screen in place of the intro, once: the
   * screen may be updated again before it is replaced. */
  void askForTheTitle(ScreenId title);

  /** @brief Plays the sound and adds the trauma of every cue the scene
   * reached since the last update; no trauma with REDUCE FLASHES. */
  void playCuesUpTo(float scene);

  /** @brief Sets the speed and the shake of the sky for that moment of the
   * scene. */
  void moveSky(float scene);

  /** @brief Paints the scene, then the texts and the logo, at that
   * moment. */
  void repaint(float real, float scene);

  IScreenNavigator* navigator_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  ISky* sky_;
  ISoundPlayer* sounds_;
  IMusicPlayer* music_;
  logging::Logger logger_;
  FlashPreference preference_;
  std::reference_wrapper<const PlayerPalette> palette_;
  std::span<const IntroFreeze> freezes_;
  SyncedClock clock_;
  CameraShake shake_;
  IntroCast cast_;
  std::vector<IntroCue> cues_;
  std::size_t nextCue_{};
  bool skipped_{};
  bool titleAsked_{};
};

}  // namespace rtype::client
