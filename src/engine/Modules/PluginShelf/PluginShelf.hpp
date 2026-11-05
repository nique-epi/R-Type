#pragma once

#include <filesystem>
#include <memory>
#include <set>
#include <span>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "IModule.hpp"
#include "SharedLibrary.hpp"

namespace rtype::engine {

/**
 * @brief The plugins of a program: the `plugin <name>` and
 * `plugin-folder <folder>` commands, the shared libraries they opened, and the
 * modules those libraries created until the Application takes them.
 *
 * A plugin line opens <folder>/<name><suffix> once: running the line again does
 * nothing, so the configuration step can run it in each of its passes. Once
 * takeModules() was called, a line naming a plugin not loaded yet is refused.
 * The libraries close when the shelf is destroyed, after the modules it still
 * holds: whoever took the modules destroys them first.
 */
class PluginShelf {
 public:
  /**
   * @param defaultFolder The folder of the plugin lines until a plugin-folder
   * line changes it.
   */
  explicit PluginShelf(std::filesystem::path defaultFolder);

  PluginShelf(const PluginShelf&) = delete;
  PluginShelf& operator=(const PluginShelf&) = delete;
  PluginShelf(PluginShelf&&) = delete;
  PluginShelf& operator=(PluginShelf&&) = delete;
  ~PluginShelf() = default;

  /**
   * @brief Registers `plugin` and `plugin-folder` in @p interpreter, which
   * must outlive the shelf.
   * @throws DuplicateCommandException if the interpreter already has one.
   */
  void registerCommands(CommandInterpreter& interpreter);

  /**
   * @returns The modules created since the last call, in the order of their
   * lines; later lines load nothing new.
   */
  [[nodiscard]] std::vector<std::unique_ptr<IModule>> takeModules();

 private:
  /**
   * @brief Runs `plugin <name>`.
   * @throws CommandRefusedException for a line that is not `plugin <name>`, or
   * that names a plugin after takeModules().
   * @throws InvalidPluginNameException, PluginNotFoundException,
   * SharedLibraryOpenException, PluginEntryNotFoundException,
   * PluginApiVersionException or NullModuleException when the plugin cannot be
   * loaded: these are not refusals, they stop the start of the program.
   */
  void load(std::span<const std::string> arguments);

  /**
   * @brief Runs `plugin-folder <folder>`.
   * @throws CommandRefusedException for a line that is not `plugin-folder
   * <folder>`, or that changes the folder after a plugin line.
   */
  void changeFolder(std::span<const std::string> arguments);

  [[nodiscard]] std::filesystem::path fileOf(const std::string& name) const;

  std::filesystem::path folder_;
  /** Declared before pending_: the modules of a library go before it. */
  std::vector<std::unique_ptr<SharedLibrary>> libraries_;
  std::vector<std::unique_ptr<IModule>> pending_;
  std::set<std::string> loaded_;
  bool closed_{false};
};

}  // namespace rtype::engine
