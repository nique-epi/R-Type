#pragma once

#include <chrono>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The notice of an extra life: EXTRA_LIFE_NOTICE_TEXT over the bay of
 * the pilot who won it, above its lives, its bottom EXTRA_LIFE_NOTICE_GAP
 * rows above the status band, rising EXTRA_LIFE_NOTICE_RISE rows over
 * EXTRA_LIFE_NOTICE_TIME, then gone.
 */
constexpr std::string_view EXTRA_LIFE_NOTICE_TEXT = "1UP";
constexpr engine::Duration EXTRA_LIFE_NOTICE_TIME =
    std::chrono::milliseconds(1500);
constexpr int EXTRA_LIFE_NOTICE_GAP = 2;
constexpr int EXTRA_LIFE_NOTICE_RISE = 6;

}  // namespace rtype::client
