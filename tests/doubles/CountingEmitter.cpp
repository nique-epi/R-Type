#include "CountingEmitter.hpp"
#include <cstddef>

void CountingEmitter::emit() {
  ++births_;
  ++alive_;
}

std::size_t CountingEmitter::aliveCount() const { return alive_; }

std::size_t CountingEmitter::births() const { return births_; }

void CountingEmitter::endOne() {
  if (alive_ > 0) {
    --alive_;
  }
}

void CountingEmitter::endAll() { alive_ = 0; }
