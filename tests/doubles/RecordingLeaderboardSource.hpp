#pragma once

#include <vector>
#include "ILeaderboardSource.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardRequest.hpp"

/** @brief Leaderboard source double that writes down what it is asked, and
 * whose page the test sets: a real source changes it at any time between two
 * updates. */
class RecordingLeaderboardSource : public rtype::client::ILeaderboardSource {
 public:
  void ask(const rtype::network::LeaderboardRequest& filter) override;
  void stopAsking() override;
  [[nodiscard]] const rtype::client::LeaderboardPage& page() const override;

  /** @brief Sets the page, under a new revision. */
  void setPage(rtype::client::LeaderboardPage page);

  [[nodiscard]] const std::vector<rtype::network::LeaderboardRequest>& asked()
      const;
  [[nodiscard]] int stopCount() const;

 private:
  rtype::client::LeaderboardPage page_;
  std::vector<rtype::network::LeaderboardRequest> asked_;
  int stopCount_ = 0;
};
