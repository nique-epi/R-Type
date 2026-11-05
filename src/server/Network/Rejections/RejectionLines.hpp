#pragma once

#include <string>
#include <vector>
#include "RejectionReport.hpp"

namespace rtype::server {

/**
 * @returns The lines of the journal that report @p report: none when nothing
 * was rejected since the previous report; otherwise a line with the count of
 * each cause, then a line for each sender the report names, then a line for
 * the senders it does not name, when there are any.
 */
[[nodiscard]] std::vector<std::string> rejectionLinesOf(
    const RejectionReport& report);

}  // namespace rtype::server
