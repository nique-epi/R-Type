#pragma once

#include <string>
#include <string_view>
#include <typeindex>
#include "ServiceRegistry.hpp"
#include "StageWork.hpp"

namespace rtype::engine {

class LoopStages;

/**
 * @brief What an Application hands one module during its install(): the
 * services of the modules installed before it, a place to provide its own, and
 * the loop stages to add its work to.
 *
 * It lives for that one install() call only, so a module looks its services up
 * there, once, and gives them to its classes through their constructors;
 * nothing is looked up while the loop runs. A service is designated by its
 * type: a module provides an EventBus, another requires an EventBus.
 */
class InstallContext {
 public:
  /**
   * @param moduleName The module being installed, named in the errors.
   * @param services Where the services are kept; it must outlive the context.
   * @param stages Where the work is added; it must outlive the context.
   */
  InstallContext(std::string moduleName, ServiceRegistry& services,
                 LoopStages& stages);

  /**
   * @brief Hands @p service, which the module owns, to the modules installed
   * after it.
   * @throws DuplicateServiceException if a module installed before already
   * provides a Service; its message names the module and the service.
   */
  template <typename Service>
  void provide(Service& service) {
    if (!services_->provide(service)) {
      refuseSecondProvider(typeid(Service));
    }
  }

  /**
   * @returns The Service a module installed before provides, or nullptr if
   * none does: an optional service is found to be absent here, without an
   * exception.
   */
  template <typename Service>
  [[nodiscard]] Service* find() const {
    return services_->find<Service>();
  }

  /**
   * @returns The Service a module installed before provides.
   * @throws MissingServiceException if none does, which stops the
   * installation; its message names the module and the service.
   */
  template <typename Service>
  [[nodiscard]] Service& require() const {
    const auto service = services_->find<Service>();
    if (service == nullptr) {
      refuseMissingService(typeid(Service));
    }
    return *service;
  }

  /**
   * @brief Adds @p work at the end of the stage named @p stageName; the work
   * of the modules installed later in the same stage runs after it.
   * @throws EmptyStageWorkException if @p work is empty.
   * @throws UnknownStageException if the loop declares no such stage.
   */
  void addWork(std::string_view stageName, StageWork work);

 private:
  [[noreturn]] void refuseSecondProvider(std::type_index service) const;
  [[noreturn]] void refuseMissingService(std::type_index service) const;

  std::string moduleName_;
  ServiceRegistry* services_;
  LoopStages* stages_;
};

}  // namespace rtype::engine
