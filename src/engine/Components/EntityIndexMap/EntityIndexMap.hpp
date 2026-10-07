#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace rtype::engine {

/**
 * @brief Maps entity indices to positions in a packed array, positions being
 * contiguous from 0 to size() - 1.
 */
class EntityIndexMap {
 public:
  /**
   * @returns The position of the entity index, or an empty optional when
   * absent.
   */
  std::optional<std::size_t> find(std::uint32_t entityIndex) const;

  /**
   * @brief Registers the entity index at position size().
   * Does nothing when the entity index is already present.
   */
  void append(std::uint32_t entityIndex);

  /**
   * @brief Removes the entity index. The entity index that was last takes the
   * vacated position, so positions stay contiguous.
   * @returns The vacated position, or an empty optional when the entity index
   * was absent.
   */
  std::optional<std::size_t> erase(std::uint32_t entityIndex);

  /** @returns The number of entity indices registered. */
  std::size_t size() const;

  /**
   * @returns The entity index registered at the position, which must be lower
   * than size().
   */
  std::uint32_t entityIndexAt(std::size_t position) const;

 private:
  static constexpr std::size_t NO_POSITION =
      std::numeric_limits<std::size_t>::max();

  std::vector<std::size_t> positionByEntityIndex_;
  std::vector<std::uint32_t> entityIndices_;
};

}  // namespace rtype::engine
