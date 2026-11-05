#include "PluginShelf.hpp"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "PathText.hpp"
#include "PluginConstants.hpp"
#include "PluginEntry.hpp"
#include "PluginShelfConstants.hpp"
#include "SharedLibrary.hpp"

namespace rtype::engine {

namespace {

bool isPluginNameCharacter(char character) {
  return std::isalnum(static_cast<unsigned char>(character)) != 0 ||
         std::ranges::find(PLUGIN_NAME_PUNCTUATION, character) !=
             PLUGIN_NAME_PUNCTUATION.end();
}

void requireValidName(const std::string& name) {
  if (name.empty() || !std::ranges::all_of(name, isPluginNameCharacter)) {
    throw InvalidPluginNameException(name);
  }
}

/**
 * @returns The function @p library exports under PLUGIN_ENTRY_NAME. The system
 * hands back its address as an object pointer, and the cast to a function
 * pointer is supported by every compiler of the project (POSIX requires it of
 * dlsym()).
 * @throws PluginEntryNotFoundException if the library exports none.
 */
PluginEntry entryOf(const std::string& name, const SharedLibrary& library) {
  void* const address = library.symbol(std::string{PLUGIN_ENTRY_NAME});
  if (address == nullptr) {
    throw PluginEntryNotFoundException(name, std::string{PLUGIN_ENTRY_NAME});
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<PluginEntry>(address);
}

/**
 * @brief Asks the entry of @p library for its version, and creates the module
 * only when that version is the one this program loads.
 * @throws PluginApiVersionException if the plugin was built for another
 * version.
 * @throws NullModuleException if the plugin created no module.
 */
std::unique_ptr<IModule> createModule(const std::string& name,
                                      const SharedLibrary& library) {
  const PluginEntry entry = entryOf(name, library);
  const std::uint32_t version = entry(nullptr);
  if (version != PLUGIN_API_VERSION) {
    throw PluginApiVersionException(name, version, PLUGIN_API_VERSION);
  }
  std::unique_ptr<IModule> module;
  static_cast<void>(entry(&module));
  if (module == nullptr) {
    throw NullModuleException();
  }
  return module;
}

void requireOneArgument(std::span<const std::string> arguments,
                        std::string_view expected) {
  if (arguments.size() != PLUGIN_ARGUMENT_COUNT) {
    throw CommandRefusedException("expected: " + std::string{expected});
  }
}

}  // namespace

PluginShelf::PluginShelf(std::filesystem::path defaultFolder)
    : folder_(std::move(defaultFolder)) {}

void PluginShelf::registerCommands(CommandInterpreter& interpreter) {
  interpreter.add(
      CommandDefinition{
          .name = std::string(PLUGIN_COMMAND_NAME),
          .arguments = "<name>",
          .help = "Loads a plugin from the plugin folder before the modules "
                  "install"},
      [this](std::span<const std::string> arguments) { load(arguments); });
  interpreter.add(
      CommandDefinition{
          .name = std::string(PLUGIN_FOLDER_COMMAND_NAME),
          .arguments = "<folder>",
          .help = "Sets the folder the plugin lines read; it comes before "
                  "every plugin line"},
      [this](std::span<const std::string> arguments) {
        changeFolder(arguments);
      });
}

std::vector<std::unique_ptr<IModule>> PluginShelf::takeModules() {
  closed_ = true;
  return std::exchange(pending_, {});
}

void PluginShelf::load(std::span<const std::string> arguments) {
  requireOneArgument(arguments, PLUGIN_LINE_SHAPE);
  const std::string& name = arguments[0];
  if (loaded_.contains(name)) {
    return;
  }
  if (closed_) {
    throw CommandRefusedException(std::string{PLUGIN_AFTER_INSTALL_REASON});
  }
  requireValidName(name);
  const std::filesystem::path file = fileOf(name);
  if (!std::filesystem::exists(file)) {
    throw PluginNotFoundException(name, genericText(file));
  }
  libraries_.push_back(std::make_unique<SharedLibrary>(file));
  pending_.push_back(createModule(name, *libraries_.back()));
  loaded_.insert(name);
}

void PluginShelf::changeFolder(std::span<const std::string> arguments) {
  requireOneArgument(arguments, PLUGIN_FOLDER_LINE_SHAPE);
  const std::filesystem::path wanted = std::filesystem::absolute(arguments[0]);
  if (wanted == folder_) {
    return;
  }
  if (!loaded_.empty() || closed_) {
    throw CommandRefusedException(std::string{FOLDER_AFTER_PLUGIN_REASON});
  }
  folder_ = wanted;
}

std::filesystem::path PluginShelf::fileOf(const std::string& name) const {
  return folder_ / (name + std::string{SharedLibrary::fileSuffix()});
}

}  // namespace rtype::engine
