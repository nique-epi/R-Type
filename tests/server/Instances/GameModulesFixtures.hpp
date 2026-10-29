#pragma once

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include "IGameModule.hpp"

namespace rtype::server::testing {

/**
 * @returns The lines of @p lines as the strings a RecordingGameModule journal
 * holds.
 */
[[nodiscard]] std::vector<std::string> journalOf(
    std::initializer_list<std::string> lines);

/**
 * @returns Two recording modules, called FIRST_MODULE_NAME and
 * SECOND_MODULE_NAME in that order, that write in @p journal; the second one's
 * start() throws when @p secondFailsToStart.
 */
[[nodiscard]] std::vector<std::unique_ptr<IGameModule>> twoRecordingModules(
    std::vector<std::string>& journal, bool secondFailsToStart);

}  // namespace rtype::server::testing
