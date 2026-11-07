#pragma once

#include <cstddef>
#include "PackDownloadOutcome.hpp"

namespace rtype::client {

/** @brief How far a download of a content pack went. */
struct PackDownloadProgress {
  /** @brief The bytes received, never more than the size announced. */
  std::size_t received{};
  PackDownloadOutcome outcome{PackDownloadOutcome::Running};
};

}  // namespace rtype::client
