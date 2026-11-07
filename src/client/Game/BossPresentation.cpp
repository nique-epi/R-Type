#include "BossPresentation.hpp"
#include <optional>
#include "BossArrivalConstants.hpp"
#include "BossArrivalEffects.hpp"
#include "BossBar.hpp"
#include "BossLook.hpp"
#include "BossPresentationConstants.hpp"
#include "BossStatus.hpp"
#include "BossTracker.hpp"
#include "EdgePulse.hpp"
#include "EffectTools.hpp"
#include "EnemyAction.hpp"
#include "FlashPreference.hpp"
#include "IAudioService.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"
#include "WarningBanner.hpp"

namespace rtype::client {

namespace {

/** @returns Whether the boss of @p tracker is arriving at @p now: an
 * Arriving status came and no status of its fight yet. */
bool isArriving(const BossTracker& tracker, engine::Duration now) {
  return tracker.sinceArrival(now).has_value() &&
         !tracker.fight().fightingSince.has_value();
}

}  // namespace

BossPresentation::BossPresentation(EffectTools tools, ISoundPlayer& sounds,
                                   IAudioService& audio)
    : effects_(tools), sounds_(&sounds), audio_(&audio) {}

void BossPresentation::apply(const network::BossStatus& status) {
  tracker_.follow(status, now_);
  followMusic();
}

void BossPresentation::apply(const network::EnemyAction& action) {
  if (action.attack != BOSS_ROAR_ATTACK ||
      action.phase != network::EnemyActionPhase::Strike ||
      !isArriving(tracker_, now_)) {
    return;
  }
  roarAt_ = now_;
  effects_.playRoar(action.entityId);
  sounds_->play(BOSS_ROAR_SOUND_ID);
}

void BossPresentation::advance(engine::Duration elapsed) {
  const std::optional<engine::Duration> before = tracker_.sinceArrival(now_);
  now_ += elapsed;
  const std::optional<engine::Duration> since = tracker_.sinceArrival(now_);
  if (!since.has_value() || !isArriving(tracker_, now_)) {
    return;
  }
  const engine::Duration previous = before.value_or(*since);
  effects_.advance(previous, *since);
  if (joltsBetween(previous, *since) > 0) {
    sounds_->play(BOSS_STEP_SOUND_ID);
  }
}

void BossPresentation::endGame() {
  gameEnded_ = true;
  followMusic();
}

BossLook BossPresentation::look(FlashPreference preference) const {
  BossLook look{.warning = {},
                .edges = {},
                .bar = bossBarAt(tracker_.fight(), now_, preference)};
  const std::optional<engine::Duration> since = tracker_.sinceArrival(now_);
  if (since.has_value()) {
    look.warning = warningBannerAt(*since, preference);
    look.edges = edgePulseAt(*since, sinceRoar(), preference);
  }
  return look;
}

void BossPresentation::followMusic() {
  const bool silence = !gameEnded_ && isArriving(tracker_, now_);
  if (silence == musicSilenced_) {
    return;
  }
  musicSilenced_ = silence;
  if (silence) {
    audio_->silenceMusic();
  } else {
    audio_->playGameMusic();
  }
}

std::optional<engine::Duration> BossPresentation::sinceRoar() const {
  if (!roarAt_.has_value()) {
    return std::nullopt;
  }
  return now_ - *roarAt_;
}

void paintBossAlert(IInterfaceCanvas& canvas, const BossLook& look) {
  paintEdgePulse(canvas, look.edges);
  paintWarningBanner(canvas, look.warning);
}

}  // namespace rtype::client
