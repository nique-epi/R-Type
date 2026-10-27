#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>

namespace rtype::engine {

/**
 * @brief The services the modules of an Application hand to each other, one
 * object per type, kept by reference: the module that provides a service owns
 * it, and must outlive every module that holds it.
 */
class ServiceRegistry {
 public:
  /**
   * @brief Keeps @p service as the service of type Service.
   * @returns false, keeping the first one, if a service of that type is
   * already kept.
   */
  template <typename Service>
  bool provide(Service& service) {
    return services_.try_emplace(std::type_index(typeid(Service)), &service)
        .second;
  }

  /** @returns The service of type Service, or nullptr if none is kept. */
  template <typename Service>
  [[nodiscard]] Service* find() const {
    const auto found = services_.find(std::type_index(typeid(Service)));
    if (found == services_.end()) {
      return nullptr;
    }
    return std::any_cast<Service*>(found->second);
  }

 private:
  /** Each value holds a Service* whose Service is the type of its key. */
  std::unordered_map<std::type_index, std::any> services_;
};

}  // namespace rtype::engine
