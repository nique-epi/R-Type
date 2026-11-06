#pragma once

#include <cstdint>
#include <vector>
#include "GameListSnapshot.hpp"
#include "IGameCatalog.hpp"
#include "ServerRow.hpp"

/** @brief Game catalog double: it writes down the server of each ask and
 * shows the games the test sets, whatever was asked. */
class RecordingGameCatalog final : public rtype::client::IGameCatalog {
 public:
  void ask(const rtype::client::ServerRow& server) override;
  [[nodiscard]] const rtype::client::GameListSnapshot& snapshot()
      const override;

  void show(rtype::client::GameListSnapshot snapshot);
  /** @returns The id of the server of each ask, in order. */
  [[nodiscard]] const std::vector<std::int64_t>& asked() const;

 private:
  rtype::client::GameListSnapshot snapshot_;
  std::vector<std::int64_t> asked_;
};
