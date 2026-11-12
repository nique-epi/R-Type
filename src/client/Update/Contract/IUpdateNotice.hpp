#pragma once

namespace rtype::client {

/**
 * @brief What the title asks the update module at each frame: whether a newer
 * release waits to be installed, and whether the notes of the release this
 * client was just updated to wait to be read.
 */
class IUpdateNotice {
 public:
  virtual ~IUpdateNotice() = default;

  /** @returns Whether a newer release holding a package for this system was
   * found, and its install has not failed. */
  [[nodiscard]] virtual bool updateAvailable() const = 0;

  /** @returns Whether this client was updated at its last start and the notes
   * of its release were not shown yet. */
  [[nodiscard]] virtual bool notesWaiting() const = 0;
};

}  // namespace rtype::client
