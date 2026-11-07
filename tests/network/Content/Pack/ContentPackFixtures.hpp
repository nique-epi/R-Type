#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include "ContentFile.hpp"

namespace rtype::network::testing {

/** @returns The drifter's sheet and the Bydomancer's sprite sheet, in the
 * order of their paths. */
[[nodiscard]] std::vector<identifiers::ContentFile> twoContentFiles();

/** @returns @p count files of one byte each, named `f00`, `f01`…, in the
 * order of their paths. */
[[nodiscard]] std::vector<identifiers::ContentFile> filesNumbered(
    std::size_t count);

/** @returns One file named ONE_LETTER_PATH holding @p size bytes. */
[[nodiscard]] std::vector<identifiers::ContentFile> oneFileOfSize(
    std::size_t size);

/**
 * @returns A pack written by hand, as a server that does not check would
 * write it: @p version, one file named @p path of one byte. It lets a test
 * hand a reader what encodeContentPack() would refuse to write.
 */
[[nodiscard]] std::vector<std::byte> handWrittenPack(std::uint8_t version,
                                                     std::string_view path);

/** @returns A pack of no file announcing @p count files. */
[[nodiscard]] std::vector<std::byte> packAnnouncing(std::uint16_t count);

}  // namespace rtype::network::testing
