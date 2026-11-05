#pragma once

#include <cstdint>
#include <vector>
#include "HeartbeatRecord.hpp"
#include "MasterDatabase.hpp"
#include "PageRequest.hpp"
#include "ServerSample.hpp"

namespace rtype::master {

/**
 * @brief The table `heartbeat_samples`: the only code that writes its SQL.
 * Each call runs in one transaction behind the lock of the MasterDatabase.
 */
class HeartbeatSampleRepository {
 public:
  explicit HeartbeatSampleRepository(MasterDatabase& database);

  /**
   * @brief Adds a sample of the heartbeat of @p record unless its server has
   * one of that second, then erases every sample older than SAMPLE_RETENTION,
   * of every server.
   */
  void add(const HeartbeatRecord& record);

  /** @returns The samples of the server @p serverId in @p page, newest
   * first; a measure the server did not take is empty. */
  [[nodiscard]] std::vector<network::ServerSample> samplesOf(
      std::int64_t serverId, const PageRequest& page);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
