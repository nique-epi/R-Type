#include "RecordingGameCatalog.hpp"
#include <cstdint>
#include <utility>
#include <vector>
#include "GameListSnapshot.hpp"
#include "ServerRow.hpp"

void RecordingGameCatalog::ask(const rtype::client::ServerRow& server) {
  asked_.push_back(server.id);
}

const rtype::client::GameListSnapshot& RecordingGameCatalog::snapshot() const {
  return snapshot_;
}

void RecordingGameCatalog::show(rtype::client::GameListSnapshot snapshot) {
  snapshot_ = std::move(snapshot);
}

const std::vector<std::int64_t>& RecordingGameCatalog::asked() const {
  return asked_;
}
