#include "ServerModeOption.hpp"
#include <span>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "ServerConstants.hpp"
#include "ServerMode.hpp"
#include "ServerModeChoice.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The mode @p name names.
 * @throws engine::InvalidLaunchOptionValueException when it names none.
 */
ServerMode modeNamed(std::string_view name) {
  if (name == SERVER_MODE_NAME) {
    return ServerMode::Server;
  }
  if (name == MASTER_MODE_NAME) {
    return ServerMode::Master;
  }
  if (name == SUPERVISOR_MODE_NAME) {
    return ServerMode::Supervisor;
  }
  throw engine::InvalidLaunchOptionValueException(std::string{MODE_OPTION},
                                                  std::string{name});
}

}  // namespace

ServerModeChoice chooseServerMode(std::span<const std::string_view> arguments) {
  if (arguments.empty() || arguments.front() != MODE_OPTION) {
    return ServerModeChoice{.mode = ServerMode::Server, .options = arguments};
  }
  if (arguments.size() < MODE_ARGUMENT_COUNT) {
    throw engine::MissingLaunchOptionValueException(std::string{MODE_OPTION});
  }
  return ServerModeChoice{.mode = modeNamed(arguments[1]),
                          .options = arguments.subspan(MODE_ARGUMENT_COUNT)};
}

}  // namespace rtype::server
