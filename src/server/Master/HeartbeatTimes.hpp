#pragma once

#include <chrono>
#include "MasterMessageConstants.hpp"
#include "ServerMasterConstants.hpp"

namespace rtype::server {

/** @brief The delays of HeartbeatJob, shorter in the tests. */
struct HeartbeatTimes {
  std::chrono::milliseconds interval{network::MASTER_HEARTBEAT_INTERVAL};
  std::chrono::milliseconds noticeGap{MINIMUM_NOTICE_GAP};
  std::chrono::milliseconds retry{REGISTRATION_RETRY};
};

}  // namespace rtype::server
