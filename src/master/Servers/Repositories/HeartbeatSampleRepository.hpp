#pragma once

#include "HeartbeatRecord.hpp"
#include "MasterDatabase.hpp"

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

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
