#include "rtff/buffer/multichannel_ring_buffer.h"

#include <cassert>

#include "rtff/buffer/buffer.h"

namespace rtff {
MultichannelRingBuffer::MultichannelRingBuffer(uint32_t read_size,
                                               uint32_t step_size,
                                               uint8_t channel_count) {
  for (auto channel_idx = 0; channel_idx < channel_count; channel_idx++) {
    buffers_.push_back(RingBuffer(read_size, step_size));
  }
}

void MultichannelRingBuffer::InitWithZeros(uint32_t frame_number) {
  for (auto& buffer : buffers_) {
    buffer.InitWithZeros(frame_number);
  }
}

RingBuffer& MultichannelRingBuffer::operator[](uint8_t channel_idx) {
  return buffers_[channel_idx];
}
const RingBuffer& MultichannelRingBuffer::operator[](
    uint8_t channel_idx) const {
  return buffers_[channel_idx];
}

void MultichannelRingBuffer::Write(const AudioBuffer& buffer) {
  assert(buffer.channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    buffers_[channel_idx].Write(buffer.data(channel_idx), buffer.frame_count());
  }
}
bool MultichannelRingBuffer::Read(AudioBuffer* buffer) {
  assert(buffer->channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    if (!buffers_[channel_idx].Read(buffer->data(channel_idx))) {
      return false;
    }
  }
  return true;
}

void MultichannelRingBuffer::Write(const Buffer<float>& buffer) {
  assert(buffer.channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    buffers_[channel_idx].Write(buffer.channel(channel_idx).data(),
                                buffer.size());
  }
}
bool MultichannelRingBuffer::Read(Buffer<float>* buffer) {
  assert(buffer->channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    if (!buffers_[channel_idx].Read(buffer->channel(channel_idx).data())) {
      return false;
    }
  }
  return true;
}

}  // namespace rtff
