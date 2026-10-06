#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "ComponentStorage.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "IComponentStorage.hpp"

namespace rtype::engine {

/**
 * @brief Owns the components of the entities of an EntityRegistry, one storage
 * per component type.
 * The EntityRegistry must outlive it. Entities must be destroyed through
 * destroy() here, otherwise their components stay behind.
 */
class ComponentRegistry {
 public:
  explicit ComponentRegistry(EntityRegistry& entities);

  /**
   * @brief Attaches a component, replacing the one of the same type if present.
   * @throws DeadEntityException when the entity is not alive.
   * @throws ComponentChangeDuringIterationException during a forEach.
   */
  template <typename T>
  void add(Entity entity, T component) {
    if (iterationDepth_ > 0) {
      throw ComponentChangeDuringIterationException();
    }
    if (!entities_.isAlive(entity)) {
      throw DeadEntityException();
    }
    createStorageIfMissing<T>().set(entity.index, std::move(component));
  }

  /**
   * @returns The component, or nullptr when absent or when the entity is not
   * alive. The pointer is invalidated by a later add or remove of the same
   * type.
   */
  template <typename T>
  T* get(Entity entity) {
    ComponentStorage<T>* storage = findStorage<T>();
    if (storage == nullptr || !entities_.isAlive(entity)) {
      return nullptr;
    }
    return storage->find(entity.index);
  }

  /** @returns true when the entity is alive and carries a component of type T.
   */
  template <typename T>
  bool has(Entity entity) const {
    const ComponentStorage<T>* storage = findStorage<T>();
    return storage != nullptr && entities_.isAlive(entity) &&
           storage->find(entity.index) != nullptr;
  }

  /**
   * @returns true when a component was removed; false when it was absent or
   * the entity is not alive.
   * @throws ComponentChangeDuringIterationException during a forEach.
   */
  template <typename T>
  bool remove(Entity entity) {
    if (iterationDepth_ > 0) {
      throw ComponentChangeDuringIterationException();
    }
    ComponentStorage<T>* storage = findStorage<T>();
    if (storage == nullptr || !entities_.isAlive(entity)) {
      return false;
    }
    return storage->erase(entity.index);
  }

  /**
   * @brief Calls callback(entity, first, second) for every alive entity that
   * has both a First and a Second component.
   *
   * Entities are visited in no particular order, each at most once. The
   * references given to the callback are only valid during that call. Entities
   * destroyed during the visit are no longer visited.
   */
  template <typename First, typename Second, typename Callback>
  void forEach(Callback&& callback) {
    ComponentStorage<First>* storage = findStorage<First>();
    if (storage == nullptr) {
      return;
    }
    const IterationScope scope(*this);
    const std::size_t count = storage->size();
    for (std::size_t position = 0; position < count; ++position) {
      const Entity entity =
          entities_.entityAt(storage->entityIndexAt(position));
      if (!isPendingDestruction(entity.index) && has<Second>(entity)) {
        callback(entity, *get<First>(entity), *get<Second>(entity));
      }
    }
  }

  /**
   * @brief Removes every component of the entity, then destroys it in the
   * EntityRegistry.
   * Does nothing when the entity is not alive: a stale handle never removes
   * the components of the entity that now uses its index.
   * While a forEach is running, the destruction is postponed until the
   * outermost forEach returns: the entity stays alive and readable until then
   * but is no longer visited.
   */
  void destroy(Entity entity);

 private:
  class IterationScope {
   public:
    explicit IterationScope(ComponentRegistry& registry);
    ~IterationScope();
    IterationScope(const IterationScope&) = delete;
    IterationScope& operator=(const IterationScope&) = delete;
    IterationScope(IterationScope&&) = delete;
    IterationScope& operator=(IterationScope&&) = delete;

   private:
    ComponentRegistry& registry_;
  };

  bool isPendingDestruction(std::uint32_t entityIndex) const;
  void destroyNow(Entity entity);
  void flushPendingDestructions();

  template <typename T>
  ComponentStorage<T>& createStorageIfMissing() {
    std::unique_ptr<IComponentStorage>& slot =
        storages_[std::type_index(typeid(T))];
    if (!slot) {
      slot = std::make_unique<ComponentStorage<T>>();
    }
    return static_cast<ComponentStorage<T>&>(*slot);
  }

  template <typename T>
  ComponentStorage<T>* findStorage() {
    const auto found = storages_.find(std::type_index(typeid(T)));
    return found == storages_.end()
               ? nullptr
               : static_cast<ComponentStorage<T>*>(found->second.get());
  }

  template <typename T>
  const ComponentStorage<T>* findStorage() const {
    const auto found = storages_.find(std::type_index(typeid(T)));
    return found == storages_.end()
               ? nullptr
               : static_cast<const ComponentStorage<T>*>(found->second.get());
  }

  EntityRegistry& entities_;
  std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>>
      storages_;
  std::size_t iterationDepth_{0};
  std::unordered_set<std::uint32_t> pendingDestructionIndices_;
};

}  // namespace rtype::engine
