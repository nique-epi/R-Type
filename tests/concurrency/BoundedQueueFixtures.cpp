#include "BoundedQueueFixtures.hpp"
#include <cstddef>
#include <vector>
#include "BoundedQueue.hpp"

std::vector<int> numbered(int first, std::size_t count) {
  std::vector<int> messages;
  messages.reserve(count);
  for (std::size_t offset = 0; offset < count; ++offset) {
    messages.push_back(first + static_cast<int>(offset));
  }
  return messages;
}

void pushAll(rtype::concurrency::BoundedQueue<int>& queue,
             const std::vector<int>& messages) {
  for (const int message : messages) {
    queue.push(message);
  }
}

std::vector<int> drained(rtype::concurrency::BoundedQueue<int>& queue) {
  std::vector<int> messages;
  queue.drainInto(messages);
  return messages;
}
