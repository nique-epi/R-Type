#pragma once

#include <cstdint>
#include <filesystem>
#include "ContentAnnouncement.hpp"
#include "LocalContent.hpp"

namespace rtype::client::testing {

/** @returns The client's own content of the tests: a fingerprint of
 * LOCAL_FINGERPRINT_BYTE, then DRAWN_ENEMY_NAME, an enemy, and
 * DRAWN_SHIP_NAME, a ship, armed with LOCAL_SHIP_GUN. */
[[nodiscard]] LocalContent testLocalContent();

/** @returns What a server playing the client's own content announces. */
[[nodiscard]] network::ContentAnnouncement localAnnouncement();

/** @returns What a server playing another content announces, of @p size
 * bytes on @p port. */
[[nodiscard]] network::ContentAnnouncement otherAnnouncement(
    std::uint32_t size, std::uint16_t port);

/** @returns The cache folder of the tests that never keep a pack. */
[[nodiscard]] std::filesystem::path unusedCacheFolder();

}  // namespace rtype::client::testing
