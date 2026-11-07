#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the loading of the content a server announced stands. */
enum class ContentStep : std::uint8_t {
  /** @brief The content is read: the session can play with it. */
  Ready,
  /** @brief The pack is downloading. */
  Loading,
  /** @brief The pack could not be downloaded: no port to ask, no answer,
   * a refusal, or every try cut. */
  DownloadFailed,
  /** @brief The pack came, but it is not the one announced, or this client
   * cannot read one of its sheets: it is never loaded. */
  Refused,
};

}  // namespace rtype::client
