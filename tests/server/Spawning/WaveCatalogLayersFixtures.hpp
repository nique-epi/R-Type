#pragma once

#include <span>
#include <string>
#include "WaveCatalog.hpp"

namespace rtype::server::testing {

/**
 * @returns The catalog of the waves files @p texts, laid in their order, the
 * files named after LAYERED_FILE_NAMES, their enemies those of the shipped
 * sheets. @p texts holds at most as many files as LAYERED_FILE_NAMES names.
 */
[[nodiscard]] WaveCatalog laid(std::span<const std::string> texts);

/**
 * @returns The message of the error reading @p texts as laid() does throws;
 * an empty text, after a failure, when it throws none.
 */
[[nodiscard]] std::string errorLaying(std::span<const std::string> texts);

}  // namespace rtype::server::testing
