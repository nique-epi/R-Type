#pragma once

#include <cstddef>
#include "IServerCatalog.hpp"
#include "ServerListSnapshot.hpp"

/** @brief Server catalog double: it counts the refreshes and shows the list
 * the test sets, as soon as it is set. */
class RecordingServerCatalog final : public rtype::client::IServerCatalog {
 public:
  void refresh() override;
  [[nodiscard]] const rtype::client::ServerListSnapshot& snapshot()
      const override;

  void show(rtype::client::ServerListSnapshot snapshot);
  [[nodiscard]] std::size_t refreshCount() const;

 private:
  rtype::client::ServerListSnapshot snapshot_;
  std::size_t refreshes_{};
};
