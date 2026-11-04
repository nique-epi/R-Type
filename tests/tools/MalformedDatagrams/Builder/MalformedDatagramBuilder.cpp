#include "MalformedDatagramBuilder.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <span>
#include <string>
#include <vector>
#include "BoundedByte.hpp"
#include "ByteWriter.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "GameStarted.hpp"
#include "Heartbeat.hpp"
#include "LobbyState.hpp"
#include "MalformedDatagramConstants.hpp"
#include "MalformedDatagramKind.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "SampleMessage.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::tools {

namespace {

/**
 * @brief The type and the size a message prefix announces, whatever the
 * payload after it holds.
 */
struct MessagePrefix {
  std::uint8_t type{};
  std::uint16_t size{};
};

template <network::EncodableMessage Message>
SampleMessage sampleOf(const Message& message) {
  return SampleMessage{.type = Message::TYPE,
                       .payload = network::encodeMessage(message)};
}

SampleMessage worldStateSample() {
  const network::WorldState state{
      .tick = SAMPLE_TICK, .players = {}, .entities = {}};
  return SampleMessage{
      .type = network::WorldState::TYPE,
      .payload = network::WorldStateCodec::encode(state).front()};
}

/** @returns One valid message of every type of version 0. */
std::vector<SampleMessage> sampleMessages() {
  return {
      sampleOf(network::ConnectionRequest{
          .cookie = NO_COOKIE, .nickname = std::string{TOOL_NICKNAME}}),
      sampleOf(network::ConnectionAccepted{.playerNumber =
                                               network::FIRST_PLAYER_NUMBER}),
      sampleOf(network::ConnectionRefused{
          .reason = network::RefusalReason::GameFull}),
      sampleOf(network::Disconnection{}),
      sampleOf(network::Heartbeat{}),
      sampleOf(network::ReadyState{.ready = true}),
      sampleOf(network::LobbyState{}),
      sampleOf(network::GameStarted{}),
      sampleOf(network::PlayerInput{.inputNumber = network::FIRST_INPUT_NUMBER,
                                    .actions = 0}),
      worldStateSample(),
      sampleOf(
          network::EntitySpawned{.entityId = SAMPLE_ENTITY_ID,
                                 .entityType = network::WireEntityType::Bydo,
                                 .x = 0.0F,
                                 .y = 0.0F}),
      sampleOf(network::EntityDestroyed{
          .entityId = SAMPLE_ENTITY_ID,
          .reason = network::DestructionReason::Killed}),
      sampleOf(network::GameEvent{.eventNumber = SAMPLE_EVENT_NUMBER,
                                  .kind = network::GameEventKind::EntityDamaged,
                                  .playerNumber = network::NO_PLAYER_NUMBER,
                                  .entityId = SAMPLE_ENTITY_ID}),
      sampleOf(network::PlayerLeft{.playerNumber = network::FIRST_PLAYER_NUMBER,
                                   .reason = network::DepartureReason::Left}),
      sampleOf(network::GameEnded{}),
      sampleOf(network::ConnectionChallenge{.cookie = SAMPLE_COOKIE}),
  };
}

std::size_t drawBetween(std::mt19937& generator, std::size_t smallest,
                        std::size_t largest) {
  return std::uniform_int_distribution<std::size_t>{smallest,
                                                    largest}(generator);
}

std::vector<std::byte> drawBytes(std::mt19937& generator, std::size_t count) {
  std::uniform_int_distribution<unsigned int> byteValue{
      0, std::numeric_limits<std::uint8_t>::max()};
  std::vector<std::byte> bytes(count);
  std::ranges::generate(bytes, [&generator, &byteValue] {
    return static_cast<std::byte>(byteValue(generator));
  });
  return bytes;
}

const SampleMessage& drawSample(std::span<const SampleMessage> samples,
                                std::mt19937& generator) {
  return samples[drawBetween(generator, 0, samples.size() - 1)];
}

/**
 * @returns The payload of the sample of @p type; every type of BOUNDED_BYTES
 * has one.
 */
std::vector<std::byte> payloadOf(std::span<const SampleMessage> samples,
                                 network::MessageType type) {
  const auto sample = std::ranges::find(samples, type, &SampleMessage::type);
  return sample == samples.end() ? std::vector<std::byte>{} : sample->payload;
}

/** @returns The PACKET_HEADER_SIZE bytes a datagram of @p header starts with.
 */
std::vector<std::byte> headerBytes(const network::PacketHeader& header) {
  std::vector<std::byte> datagram = network::DatagramCodec::encode(
      header, network::MessageType::Heartbeat, {});
  datagram.resize(network::PACKET_HEADER_SIZE);
  return datagram;
}

/**
 * @returns A datagram of @p header, then one message announced by @p prefix
 * and followed by @p payload, whatever its size.
 */
std::vector<std::byte> datagramOf(const network::PacketHeader& header,
                                  MessagePrefix prefix,
                                  std::span<const std::byte> payload) {
  network::ByteWriter writer;
  writer.writeBytes(headerBytes(header));
  writer.writeUint8(prefix.type);
  writer.writeUint16(prefix.size);
  writer.writeBytes(payload);
  return writer.bytes();
}

}  // namespace

MalformedDatagramBuilder::MalformedDatagramBuilder(std::uint32_t seed)
    : generator_(seed), samples_(sampleMessages()) {}

std::vector<std::byte> MalformedDatagramBuilder::build(
    MalformedDatagramKind kind, const network::PacketHeader& header) {
  switch (kind) {
    case MalformedDatagramKind::Truncated:
      return truncated(header);
    case MalformedDatagramKind::TooLong:
      return tooLong(header);
    case MalformedDatagramKind::Oversized:
      return oversized();
    case MalformedDatagramKind::UnknownType:
      return unknownType(header);
    case MalformedDatagramKind::LyingSize:
      return lyingSize(header);
    case MalformedDatagramKind::ExtremeValue:
      return extremeValue(header);
    case MalformedDatagramKind::RandomBytes:
      return anyBytes();
  }
  return anyBytes();
}

std::vector<std::byte> MalformedDatagramBuilder::truncated(
    const network::PacketHeader& header) {
  const SampleMessage& sample = drawSample(samples_, generator_);
  std::vector<std::byte> datagram =
      network::DatagramCodec::encode(header, sample.type, sample.payload);
  datagram.resize(drawBetween(generator_, 0, datagram.size() - 1));
  return datagram;
}

std::vector<std::byte> MalformedDatagramBuilder::tooLong(
    const network::PacketHeader& header) {
  const SampleMessage& sample = drawSample(samples_, generator_);
  std::vector<std::byte> payload = sample.payload;
  const std::vector<std::byte> extra = drawBytes(
      generator_, drawBetween(generator_, 1,
                              network::MESSAGE_PAYLOAD_LIMIT - payload.size()));
  payload.insert(payload.end(), extra.begin(), extra.end());
  return network::DatagramCodec::encode(header, sample.type, payload);
}

std::vector<std::byte> MalformedDatagramBuilder::oversized() {
  return drawBytes(generator_,
                   drawBetween(generator_, network::MAX_DATAGRAM_SIZE + 1,
                               OVERSIZED_DATAGRAM_LIMIT));
}

std::vector<std::byte> MalformedDatagramBuilder::unknownType(
    const network::PacketHeader& header) {
  const auto type = static_cast<std::uint8_t>(
      drawBetween(generator_, FIRST_UNKNOWN_MESSAGE_TYPE,
                  std::numeric_limits<std::uint8_t>::max()));
  const std::vector<std::byte> payload =
      drawBytes(generator_, drawBetween(generator_, 0, UNKNOWN_PAYLOAD_LIMIT));
  return datagramOf(
      header,
      MessagePrefix{.type = type,
                    .size = static_cast<std::uint16_t>(payload.size())},
      payload);
}

std::vector<std::byte> MalformedDatagramBuilder::lyingSize(
    const network::PacketHeader& header) {
  const SampleMessage& sample = drawSample(samples_, generator_);
  const auto announced = static_cast<std::uint16_t>(
      drawBetween(generator_, sample.payload.size() + 1,
                  std::numeric_limits<std::uint16_t>::max()));
  return datagramOf(
      header,
      MessagePrefix{.type = static_cast<std::uint8_t>(sample.type),
                    .size = announced},
      sample.payload);
}

std::vector<std::byte> MalformedDatagramBuilder::extremeValue(
    const network::PacketHeader& header) {
  const BoundedByte& field =
      BOUNDED_BYTES.at(drawBetween(generator_, 0, BOUNDED_BYTES.size() - 1));
  std::vector<std::byte> payload = payloadOf(samples_, field.type);
  payload.at(field.offset) = std::byte{OUT_OF_RANGE_BYTE};
  return network::DatagramCodec::encode(header, field.type, payload);
}

std::vector<std::byte> MalformedDatagramBuilder::anyBytes() {
  return drawBytes(generator_,
                   drawBetween(generator_, 0, network::MAX_DATAGRAM_SIZE));
}

}  // namespace rtype::tools
