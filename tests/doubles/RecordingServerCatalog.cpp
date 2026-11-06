#include "RecordingServerCatalog.hpp"
#include <cstddef>
#include <utility>
#include "ServerListSnapshot.hpp"

void RecordingServerCatalog::refresh() { ++refreshes_; }

const rtype::client::ServerListSnapshot& RecordingServerCatalog::snapshot()
    const {
  return snapshot_;
}

void RecordingServerCatalog::show(rtype::client::ServerListSnapshot snapshot) {
  snapshot_ = std::move(snapshot);
}

std::size_t RecordingServerCatalog::refreshCount() const { return refreshes_; }
