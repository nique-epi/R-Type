#pragma once

#include <cstdint>
#include <random>

/**
 * @brief Stand-in for the losses of a network: draws from a seed whether each
 * datagram is lost, each with the same chance whatever came before, so a test
 * sees the same losses at every run with the same standard library.
 */
class SeededLoss {
 public:
  SeededLoss(std::uint32_t seed, double lossRate);

  /** @returns Whether the next datagram is lost. */
  [[nodiscard]] bool losesNext();

 private:
  std::mt19937 generator_;
  std::bernoulli_distribution loss_;
};
