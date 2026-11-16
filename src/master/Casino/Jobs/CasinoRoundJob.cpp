#include "CasinoRoundJob.hpp"
#include <exception>
#include <stop_token>
#include <string>
#include <utility>
#include <vector>
#include "CasinoSettings.hpp"
#include "DrawnRound.hpp"
#include "InterruptibleSleep.hpp"
#include "MasterConstants.hpp"
#include "RouletteDealer.hpp"
#include "RoundClock.hpp"

namespace rtype::master {

CasinoRoundJob::CasinoRoundJob(RouletteDealer& dealer, CasinoSettings settings,
                               RoundClock clock)
    : dealer_(dealer),
      settings_(settings),
      clock_(std::move(clock)),
      logger_(std::string{MASTER_LOGGER_NAME}),
      casinoClock_([this](const std::stop_token& stop) { run(stop); }) {}

void CasinoRoundJob::run(const std::stop_token& stop) {
  while (!stop.stop_requested()) {
    tell(pass());
    concurrency::sleepUnlessStopped(stop, clock_.interval);
  }
}

std::vector<DrawnRound> CasinoRoundJob::pass() {
  try {
    std::vector<DrawnRound> drawn =
        missedRoundsDue_ ? dealer_.drawMissedRounds(clock_.now(), settings_)
                         : dealer_.drawDueRounds(clock_.now(), settings_);
    missedRoundsDue_ = false;
    return drawn;
  } catch (const std::exception& error) {
    logger_.error("the rounds of the casino were not drawn: ", error.what());
    missedRoundsDue_ = true;
    return {};
  }
}

void CasinoRoundJob::tell(const std::vector<DrawnRound>& rounds) const {
  for (const DrawnRound& round : rounds) {
    logger_.info("round ", round.place.round, " of room ",
                 int{round.place.room}, " drew ", int{round.winningNumber});
  }
}

}  // namespace rtype::master
