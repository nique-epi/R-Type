#pragma once

#include <cstdint>
#include <optional>
#include "BossArrivalEffects.hpp"
#include "BossLook.hpp"
#include "BossStatus.hpp"
#include "BossTracker.hpp"
#include "EffectTools.hpp"
#include "EnemyAction.hpp"
#include "FlashPreference.hpp"
#include "IAudioService.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the client shows of a boss from its Arriving status: the music
 * of the game silenced, the alarm and then the music of the boss until the
 * game ends, the WARNING screen and the red edges (see warningBannerAt(),
 * edgePulseAt()), the rumble and the steps of the camera with a sound at each
 * step, at its roar (the strike of attack BOSS_ROAR_ATTACK during the arrival)
 * its shockwaves, its flash and its sound, then its bar (see bossBarAt()). The
 * effects, the sounds and the audio must outlive it.
 */
class BossPresentation {
 public:
  BossPresentation(EffectTools tools, ISoundPlayer& sounds,
                   IAudioService& audio);

  /** @brief Follows the status of the boss; Arriving silences the music,
   * plays the alarm and asks for the music of the boss, which the fight
   * keeps. */
  void apply(const network::BossStatus& status);

  /** @brief Plays the roar when @p action is the roar of the boss. */
  void apply(const network::EnemyAction& action);

  /** @brief Moves its clock on by @p elapsed, and plays what falls in
   * between: shakes and steps. */
  void advance(engine::Duration elapsed);

  /** @brief The game ended: the music of the game comes back if the music
   * of the boss replaced it. */
  void endGame();

  /** @returns What the boss lays over the playfield right now, with the
   * flashes @p preference allows: the WARNING screen and the edges once an
   * Arriving status came, the bar once a status of its fight came, even when
   * the Arriving status was lost. */
  [[nodiscard]] BossLook look(FlashPreference preference) const;

 private:
  /** @brief Silences the music, plays the alarm and asks for the music of
   * the boss when it arrives in a game not ended, and brings the music of the
   * game back when the game ends, each once. */
  void followMusic();

  [[nodiscard]] std::optional<engine::Duration> sinceRoar() const;

  BossTracker tracker_;
  BossArrivalEffects effects_;
  ISoundPlayer* sounds_;
  IAudioService* audio_;
  engine::Duration now_{};
  std::optional<engine::Duration> roarAt_;
  /** @brief When the newest charge was signalled, and the row of the
   * interface picture it runs along. */
  std::optional<engine::Duration> chargeSignalledAt_;
  int chargeRow_{};
  /** @brief The network id of the boss drawn shielded, until its fight. */
  std::optional<std::uint32_t> shielded_;
  bool bossMusicStarted_{false};
  bool gameEnded_{false};
};

/** @brief Paints the edges, then the WARNING screen over them; the bar is
 * painted with the notices, after the mission banner (see GameScreen). */
void paintBossAlert(IInterfaceCanvas& canvas, const BossLook& look);

}  // namespace rtype::client
