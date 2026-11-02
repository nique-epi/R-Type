#include "ConfigurationLog.hpp"
#include <string>
#include <string_view>
#include "ConfigurationReport.hpp"
#include "Logger.hpp"

namespace rtype::engine {

void logConfigurationReport(const ConfigurationReport& report,
                            std::string_view fileName,
                            const logging::Logger& logger) {
  if (report.fileFound) {
    logger.info("configuration read from ", fileName, ": ",
                report.executedLines, " commands run");
  } else {
    logger.info(fileName, " not found, the defaults apply");
  }
  for (const std::string& refusal : report.refusals) {
    logger.warn("configuration: ", refusal);
  }
}

}  // namespace rtype::engine
