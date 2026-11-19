#include "BossPresentation.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "AppearanceReference.hpp"
#include "BossArrivalConstants.hpp"
#include "BossArrivalEffects.hpp"
#include "BossBar.hpp"
#include "BossLook.hpp"
#include "BossPresentationConstants.hpp"
#include "BossStatus.hpp"
#include "BossTracker.hpp"
#include "ComponentRegistry.hpp"
#include "EdgePulse.hpp"
#include "EffectTools.hpp"
#include "EnemyAction.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityDestroyed.hpp"
#include "FlashPreference.hpp"
#include "IAudioService.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "InterfacePixel.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WarningBanner.hpp"
#include "WatchOut.hpp"

namespace rtype::client {

namespace {

/** @returns Whether the boss of @p tracker is arriving at @p now: an
 * Arriving status came and no status of its fight yet. */
bool isArriving(const BossTracker& tracker, engine::Duration now) {
  return tracker.sinceArrival(now).has_value() &&
         !tracker.fight().fightingSince.has_value();
}

/** @returns The name the sheet of the entity of network id @p bossId gives
 * its bar; empty when the client has no such entity, or its sheet names
 * none. */
std::string bossNameOf(engine::ComponentRegistry& components,
                       std::uint32_t bossId) {
  std::string name;
  components.forEach<game::NetworkIdentity, AppearanceReference>(
      [&name, bossId](engine::Entity, const game::NetworkIdentity& identity,
                      const AppearanceReference& reference) {
        if (identity.networkId == bossId && reference.appearance != nullptr) {
          name = reference.appearance->bossName;
        }
      });
  return name;
}

}  // namespace

BossPresentation::BossPresentation(EffectTools tools, ISoundPlayer& sounds,
                                   IAudioService& audio)
    : effects_(tools),
      components_(&tools.components.get()),
      sounds_(&sounds),
      audio_(&audio) {}

void BossPresentation::apply(const network::BossStatus& status) {
  if (status.stage == network::BossStage::Arriving) {
    roarAt_.reset();
    chargeSignalledAt_.reset();
    bossName_.clear();
  }
  if (bossName_.empty()) {
    bossName_ = bossNameOf(*components_, status.bossEntityId);
  }
  tracker_.follow(status, now_);
  if (status.stage != network::BossStage::Arriving) {
    bossInPlay_ = status.bossEntityId;
  }
  if (status.stage != network::BossStage::Arriving && shielded_.has_value()) {
    effects_.shield(*shielded_, false);
    shielded_.reset();
  }
  followMusic();
}

void BossPresentation::apply(const network::EnemyAction& action) {
  if (action.attack == BOSS_CHARGE_ATTACK &&
      action.phase == network::EnemyActionPhase::Signal) {
    const std::optional<game::Position> boss =
        effects_.positionOf(action.entityId);
    if (boss.has_value()) {
      chargeSignalledAt_ = now_;
      chargeRow_ = interfacePixelAt({boss->x, boss->y}).y;
    }
    return;
  }
  if (action.attack != BOSS_ROAR_ATTACK || !isArriving(tracker_, now_)) {
    return;
  }
  effects_.shield(action.entityId, true);
  shielded_ = action.entityId;
  if (action.phase != network::EnemyActionPhase::Strike) {
    return;
  }
  roarAt_ = now_;
  effects_.playRoar(action.entityId);
  sounds_->play(BOSS_ROAR_SOUND_ID);
}

void BossPresentation::apply(const network::EntityDestroyed& destroyed) {
  if (destroyed.reason != network::DestructionReason::Removed ||
      bossInPlay_ != destroyed.entityId) {
    return;
  }
  tracker_.forget();
  bossInPlay_.reset();
  bossName_.clear();
  roarAt_.reset();
  chargeSignalledAt_.reset();
  if (bossMusicStarted_) {
    bossMusicStarted_ = false;
    audio_->playGameMusic();
  }
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

void BossPresentation::followLevel() { playsALevel_ = true; }

void BossPresentation::endGame() {
  gameEnded_ = true;
  followMusic();
}

BossLook BossPresentation::look(FlashPreference preference) const {
  BossLook look{
      .warning = {},
      .edges = {},
      .bar = bossBarAt(tracker_.fight(), now_, preference),
      .watchOut = watchOutAt(
          WatchOutCue{
              .row = chargeRow_,
              .sinceSignal =
                  chargeSignalledAt_.has_value()
                      ? std::optional<engine::Duration>{now_ -
                                                        *chargeSignalledAt_}
                      : std::nullopt},
          preference)};
  look.bar.name = bossName_;
  const std::optional<engine::Duration> since = tracker_.sinceArrival(now_);
  if (since.has_value()) {
    look.warning = warningBannerAt(*since, preference);
    look.edges = edgePulseAt(*since, sinceRoar(), preference);
  }
  return look;
}

void BossPresentation::followMusic() {
  const bool arriving = !gameEnded_ && isArriving(tracker_, now_);
  if (arriving && !bossMusicStarted_) {
    bossMusicStarted_ = true;
    audio_->silenceMusic();
    sounds_->play(BOSS_ALARM_SOUND_ID);
    audio_->playBossMusic();
    return;
  }
  const bool bossGone =
      gameEnded_ ||
      (playsALevel_ && tracker_.fight().stage == network::BossStage::Defeated);
  if (bossGone && bossMusicStarted_) {
    bossMusicStarted_ = false;
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
  paintWatchOut(canvas, look.watchOut);
}

}  // namespace rtype::client
