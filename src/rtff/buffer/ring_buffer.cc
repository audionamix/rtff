#include "rtff/buffer/ring_buffer.h"

#include "rtff/buffer/audio_buffer.h"
#include "rtff/buffer/buffer.h"

namespace rtff {

//-----------------------------------
//-----------------------------------
// RingBuffer
//-----------------------------------
//-----------------------------------
RingBuffer::RingBuffer(uint32_t container_size) {
  write_index_ = 0;
  read_index_ = 0;
  available_data_size_ = 0;
  buffer_.resize(container_size);
}

void RingBuffer::InitWithZeros(uint32_t count) {
  if (write_index_ + count > buffer_.size()) {
    auto remaining_size = buffer_.size() - write_index_;
    std::fill(buffer_.data() + write_index_,
              buffer_.data() + write_index_ + remaining_size, 0);
    std::fill(buffer_.data(), buffer_.data() + (count - remaining_size), 0);
    write_index_ = (count - remaining_size);
  } else {
    std::fill(buffer_.data() + write_index_,
              buffer_.data() + write_index_ + count, 0);
    write_index_ += count;
  }
  available_data_size_ += count;
}

void RingBuffer::Write(const float* data, uint32_t frame_count) {
  auto write_size = frame_count;
  if (write_index_ + write_size > buffer_.size()) {
    // When we reach the end of the buffer
    auto remaining_size = buffer_.size() - write_index_;
    std::copy(data, data + remaining_size, buffer_.data() + write_index_);
    std::copy(data + remaining_size, data + write_size, buffer_.data());
    write_index_ = (write_size - remaining_size);
  } else {
    // we have enough size remaining
    std::copy(data, data + write_size, buffer_.data() + write_index_);
    write_index_ += write_size;
  }
  available_data_size_ += write_size;
}

bool RingBuffer::Read(float* data, uint32_t frame_count) {
  auto read_size = frame_count;
  if (available_data_size_ < read_size) {
    return false;
  }

  if (read_index_ + read_size > buffer_.size()) {
    auto remaining_size = buffer_.size() - read_index_;
    std::copy(buffer_.data() + read_index_,
              buffer_.data() + read_index_ + remaining_size, data);
    std::copy(buffer_.data(), buffer_.data() + (read_size - remaining_size),
              data + remaining_size);
    read_index_ += read_size;
    if (read_index_ > buffer_.size()) {
      read_index_ -= buffer_.size();
    }
  } else {
    // default read
    std::copy(buffer_.data() + read_index_,
              buffer_.data() + read_index_ + read_size, data);
    read_index_ += read_size;
  }
  available_data_size_ -= read_size;

  return true;
}

//-----------------------------------
//-----------------------------------
// Multichannel Ring Buffer
//-----------------------------------
//-----------------------------------
MultichannelRingBuffer::MultichannelRingBuffer(uint32_t container_size,
                                               uint8_t channel_count) {
  for (auto channel_idx = 0; channel_idx < channel_count; channel_idx++) {
    buffers_.push_back(RingBuffer(container_size));
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

void MultichannelRingBuffer::Write(const AudioBuffer& buffer,
                                   uint32_t frame_count) {
  assert(buffer.channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    buffers_[channel_idx].Write(buffer.data(channel_idx), frame_count);
  }
}
bool MultichannelRingBuffer::Read(AudioBuffer* buffer, uint32_t frame_count) {
  assert(buffer->channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    if (!buffers_[channel_idx].Read(buffer->data(channel_idx), frame_count)) {
      return false;
    }
  }
  return true;
}

void MultichannelRingBuffer::Write(const Buffer<float>& buffer,
                                   uint32_t frame_count) {
  assert(buffer.channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    buffers_[channel_idx].Write(buffer.channel(channel_idx).data(),
                                frame_count);
  }
}
bool MultichannelRingBuffer::Read(Buffer<float>* buffer, uint32_t frame_count) {
  assert(buffer->channel_count() == buffers_.size());
  for (auto channel_idx = 0; channel_idx < buffers_.size(); channel_idx++) {
    if (!buffers_[channel_idx].Read(buffer->channel(channel_idx).data(),
                                    frame_count)) {
      return false;
    }
  }
  return true;
}

}  // namespace rtff
