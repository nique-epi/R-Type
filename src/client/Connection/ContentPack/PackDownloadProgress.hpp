#pragma once

#include <cstddef>
#include "PackDownloadOutcome.hpp"

namespace rtype::client {

/** @brief How far a download of a content pack went. */
struct PackDownloadProgress {
  std::size_t received{};
  /** @brief The size the server announced, never exceeded. */
  std::size_t size{};
  PackDownloadOutcome outcome{PackDownloadOutcome::Running};
};

}  // namespace rtype::client
