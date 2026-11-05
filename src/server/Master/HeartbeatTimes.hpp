#pragma once

#include <chrono>
#include "ServerMasterConstants.hpp"

namespace rtype::server {

/** @brief The delays of HeartbeatJob, shorter in the tests. */
struct HeartbeatTimes {
  std::chrono::milliseconds interval{HEARTBEAT_INTERVAL};
  std::chrono::milliseconds noticeGap{MINIMUM_NOTICE_GAP};
  std::chrono::milliseconds retry{REGISTRATION_RETRY};
};

}  // namespace rtype::server
