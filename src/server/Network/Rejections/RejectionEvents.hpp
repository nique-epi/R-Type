#pragma once

#include <vector>
#include "RejectionEvent.hpp"
#include "RejectionReport.hpp"

namespace rtype::server {

/**
 * @returns The events of the journal that report @p report: none when nothing
 * was rejected since the previous report; otherwise an event with the count of
 * each cause, then an event for each sender the report names, then an event
 * for the senders it does not name, when there are any. Each event names the
 * causes it counts as fields, one per cause rejected at least once. The
 * events point into @p report, which must outlive them.
 */
[[nodiscard]] std::vector<RejectionEvent> rejectionEventsOf(
    const RejectionReport& report);

}  // namespace rtype::server
