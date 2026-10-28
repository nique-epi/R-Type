#pragma once

#include <vector>
#include "IDatagramSender.hpp"
#include "OutgoingDatagram.hpp"

/** @brief Datagram sender double that keeps every datagram it is given, in
 * order, and sends none. */
class RecordingDatagramSender : public rtype::server::IDatagramSender {
 public:
  void queueDatagram(rtype::network::OutgoingDatagram datagram) override;

  /** @returns The datagrams queued so far, oldest first. */
  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& sent()
      const;

 private:
  std::vector<rtype::network::OutgoingDatagram> sent_;
};
