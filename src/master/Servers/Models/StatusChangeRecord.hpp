#pragma once

#include <cstdint>
#include "DatedStatus.hpp"

namespace rtype::master {

/** @brief A change of status of a server, as the table status_changes keeps
 * it. */
struct StatusChangeRecord {
  std::int64_t serverId{};
  network::DatedStatus change;
};

}  // namespace rtype::master
