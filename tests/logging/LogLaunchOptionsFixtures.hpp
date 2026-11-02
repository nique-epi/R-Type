#pragma once

#include <filesystem>
#include <span>
#include <string_view>
#include "LogLaunchOptions.hpp"

/**
 * @returns The logging options @p arguments give, each read in turn as a
 * program reads them, for a launch whose default journal is
 * @p defaultJournal. Fails the running test at an argument that is no
 * logging option.
 */
[[nodiscard]] rtype::logging::LogLaunchOptions logOptionsOf(
    std::span<const std::string_view> arguments,
    const std::filesystem::path& defaultJournal);
