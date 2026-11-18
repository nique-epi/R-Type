#pragma once

#include <stop_token>
#include <thread>
#include <vector>
#include "CasinoSettings.hpp"
#include "DrawnRound.hpp"
#include "Logger.hpp"
#include "RouletteDealer.hpp"
#include "RoundClock.hpp"

namespace rtype::master {

/**
 * @brief The job that draws the rounds of the casino, on the CasinoClock
 * thread, which it owns: it asks the RouletteDealer for the rounds due as soon
 * as it is built, then every interval of its clock, and writes each number
 * drawn in the journal, `round <round> of room <room> drew <number>`. Its
 * first pass also draws the rounds missed while the master was down
 * (RouletteDealer::drawMissedRounds()); the next ones only the current round.
 *
 * A pass that fails, the database locked or full, is written in the journal
 * and tried again an interval later, looking for missed rounds again: an
 * exception that left the thread would end the master. CasinoClock stops when
 * the job is destroyed, once the pass in progress is done; a sleep between two
 * passes ends at once.
 */
class CasinoRoundJob {
 public:
  /** @param dealer Draws the rounds; it must outlive the job. @p settings are
   * those of the casino at the start of the master. */
  CasinoRoundJob(RouletteDealer& dealer, CasinoSettings settings,
                 RoundClock clock);

  CasinoRoundJob(const CasinoRoundJob&) = delete;
  CasinoRoundJob& operator=(const CasinoRoundJob&) = delete;
  CasinoRoundJob(CasinoRoundJob&&) = delete;
  CasinoRoundJob& operator=(CasinoRoundJob&&) = delete;
  ~CasinoRoundJob() = default;

 private:
  void run(const std::stop_token& stop);
  /** @returns The rounds one pass drew, none when it failed, which is
   * written in the journal. */
  [[nodiscard]] std::vector<DrawnRound> pass();
  /** @brief Writes each round of @p rounds in the journal. */
  void tell(const std::vector<DrawnRound>& rounds) const;

  RouletteDealer& dealer_;
  CasinoSettings settings_;
  RoundClock clock_;
  logging::Logger logger_;
  /** @brief Whether the next pass looks for missed rounds: at the start, and
   * after a pass that failed. Read on CasinoClock only. */
  bool missedRoundsDue_{true};
  std::jthread casinoClock_;
};

}  // namespace rtype::master
