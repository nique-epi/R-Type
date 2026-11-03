#pragma once

#include <optional>
#include "BoundedQueue.hpp"
#include "LeaderboardAsk.hpp"
#include "LeaderboardReply.hpp"
#include "LeaderboardRequest.hpp"
#include "ResultsDatabaseFixture.hpp"
#include "ResultsDatabaseTestConstants.hpp"

namespace rtype::server::testing {

/** @brief An archive on the database of ResultsDatabaseFixture, and a queue for
 * its answers. */
class ResultsArchiveFixture : public ResultsDatabaseFixture {
 protected:
  /** @returns A request for @p board from ACE, answered on the queue of the
   * fixture. */
  [[nodiscard]] LeaderboardAsk askFor(const network::LeaderboardRequest& board);

  /** @returns The next answer, or nothing after THREAD_TIMEOUT. */
  [[nodiscard]] std::optional<LeaderboardReply> nextReply();

 private:
  concurrency::BoundedQueue<LeaderboardReply> replies_{REPLY_QUEUE_ROOM};
};

}  // namespace rtype::server::testing
