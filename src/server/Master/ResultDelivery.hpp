#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief What became of a result sent to the master. */
enum class ResultDelivery : std::uint8_t {
  /** @brief The master keeps it: it is never sent again. */
  Accepted,
  /** @brief The master refused it for good (FOR_GOOD_REFUSAL_STATUS): it is
   * never sent again either. */
  Refused,
  /** @brief No answer, or a refusal that may not last: it waits, and the
   * results after it wait with it, for the next heartbeat. */
  Later,
};

}  // namespace rtype::server
