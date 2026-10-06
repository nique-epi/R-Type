#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "EntityIndexMap/EntityIndexMap.hpp"
#include "IComponentStorage.hpp"

namespace rtype::engine {

/**
 * @brief Contiguous storage of the components of type T, found by entity
 * index.
 */
template <typename T>
class ComponentStorage final : public IComponentStorage {
 public:
  /** @brief Stores the component of the entity index, replacing the previous
   * one. */
  void set(std::uint32_t entityIndex, T component) {
    if (T* existing = find(entityIndex)) {
      *existing = std::move(component);
      return;
    }
    entityIndexMap_.append(entityIndex);
    components_.push_back(std::move(component));
  }

  /** @returns The component, or nullptr when the entity index has none. */
  T* find(std::uint32_t entityIndex) {
    const std::optional<std::size_t> position =
        entityIndexMap_.find(entityIndex);
    return position.has_value() ? &components_[*position] : nullptr;
  }

  const T* find(std::uint32_t entityIndex) const {
    const std::optional<std::size_t> position =
        entityIndexMap_.find(entityIndex);
    return position.has_value() ? &components_[*position] : nullptr;
  }

  /** @returns The number of components stored. */
  std::size_t size() const { return entityIndexMap_.size(); }

  /**
   * @returns The entity index owning the component at the position, which must
   * be lower than size().
   */
  std::uint32_t entityIndexAt(std::size_t position) const {
    return entityIndexMap_.entityIndexAt(position);
  }

  /** @returns true when a component was removed. */
  bool erase(std::uint32_t entityIndex) override {
    const std::optional<std::size_t> vacated =
        entityIndexMap_.erase(entityIndex);
    if (!vacated.has_value()) {
      return false;
    }
    if (*vacated != components_.size() - 1) {
      components_[*vacated] = std::move(components_.back());
    }
    components_.pop_back();
    return true;
  }

 private:
  EntityIndexMap entityIndexMap_;
  std::vector<T> components_;
};

}  // namespace rtype::engine
