#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "ContentPackDownload.hpp"
#include "PackDownloadProgress.hpp"
#include "PackDownloadTarget.hpp"

namespace rtype::client::testing {

/** @returns How @p download ended, once it ended or DOWNLOAD_WAIT passed. */
[[nodiscard]] PackDownloadProgress endOf(const ContentPackDownload& download);

/** @returns The pack of the lancer as the body an HTTP answer carries. */
[[nodiscard]] std::string lancerBody();

/** @returns The pack at PACK_PATH on the loopback and @p port, of @p size. */
[[nodiscard]] PackDownloadTarget packAt(std::uint16_t port, std::size_t size);

}  // namespace rtype::client::testing
