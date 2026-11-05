#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace rtype::engine {

/**
 * @brief A shared library opened for the life of this object: dlopen() on
 * Linux and macOS (SharedLibraryPosix.cpp), LoadLibraryExW() on Windows
 * (SharedLibraryWindows.cpp). Destroying it closes the library: everything
 * made by its code (objects, functions kept in std::function, std::any) must
 * be gone by then.
 */
class SharedLibrary {
 public:
  /**
   * @brief Opens @p file, binding every symbol now so a missing one fails
   * here, not at its first call.
   * @throws SharedLibraryOpenException naming @p file and the system's reason.
   */
  explicit SharedLibrary(const std::filesystem::path& file);

  /** @brief Closes the library; never throws. */
  ~SharedLibrary();

  SharedLibrary(const SharedLibrary&) = delete;
  SharedLibrary& operator=(const SharedLibrary&) = delete;
  SharedLibrary(SharedLibrary&&) = delete;
  SharedLibrary& operator=(SharedLibrary&&) = delete;

  /** @returns The address the library exports under @p name, or nullptr. */
  [[nodiscard]] void* symbol(const std::string& name) const;

  /** @returns The suffix of a library file on this system: ".so" or ".dll". */
  [[nodiscard]] static std::string_view fileSuffix();

 private:
  void* handle_;
};

}  // namespace rtype::engine
