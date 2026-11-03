#pragma once

#include <cstddef>
#include <vector>
#include "GameResult.hpp"
#include "IResultsArchive.hpp"
#include "LeaderboardAsk.hpp"

/** @brief Archive double that keeps what it is given, on the calling thread,
 * and touches no disk. */
class RecordingResultsArchive final : public rtype::server::IResultsArchive {
 public:
  void record(rtype::server::GameResult result) override;
  void ask(rtype::server::LeaderboardAsk ask) override;

  /** @brief Answers each ask kept so far with @p payload, as the storage thread
   * does, and forgets them. */
  void answerEveryAsk(const std::vector<std::byte>& payload);

  [[nodiscard]] const std::vector<rtype::server::GameResult>& results() const;
  [[nodiscard]] const std::vector<rtype::server::LeaderboardAsk>& asks() const;

 private:
  std::vector<rtype::server::GameResult> results_;
  std::vector<rtype::server::LeaderboardAsk> asks_;
};
