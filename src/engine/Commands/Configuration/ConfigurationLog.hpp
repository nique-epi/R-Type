#pragma once

#include <string_view>
#include "ConfigurationReport.hpp"
#include "Logger.hpp"

namespace rtype::engine {

/**
 * @brief Tells the journal what reading the configuration did: that
 * @p fileName was not found and the defaults apply, or how many commands ran,
 * and each refused line as a warning. An empty @p fileName is a program that
 * read no file: only its refusals are told.
 *
 * Call it once the journal is open: the configuration is read before the
 * modules start, and the logger module opens the journal when it starts.
 */
void logConfigurationReport(const ConfigurationReport& report,
                            std::string_view fileName,
                            const logging::Logger& logger);

}  // namespace rtype::engine
