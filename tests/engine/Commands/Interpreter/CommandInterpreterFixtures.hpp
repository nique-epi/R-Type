#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "CommandHandler.hpp"

/**
 * @returns A handler that appends each word it is given to @p received.
 */
rtype::engine::CommandHandler recordingHandler(
    std::vector<std::string>& received);

/**
 * @returns A handler that refuses its words with @p reason.
 */
rtype::engine::CommandHandler refusingHandler(std::string_view reason);

/**
 * @returns The line that runs @p command with the two words of the tests.
 */
std::string lineRunning(std::string_view command);
