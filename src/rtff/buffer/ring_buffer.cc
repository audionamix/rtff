#include "rtff/buffer/ring_buffer.h"

#include "rtff/buffer/audio_buffer.h"
#include "rtff/buffer/buffer.h"

namespace rtff {

RingBuffer::RingBuffer(uint32_t read_size, uint32_t step_size) {
  read_size_ = read_size;
  step_size_ = step_size;
  write_index_ = 0;
  read_index_ = 0;
  available_data_size_ = 0;

  // the buffer size is arbitrary.
  // We should give a way to initialize it to a given value to
  // avoid allocating uncessary memory
  buffer_.resize(read_size_ * 8);
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

bool RingBuffer::Read(float* data) {
  if (available_data_size_ < read_size_) {
    return false;
  }

  if (read_index_ + read_size_ > buffer_.size()) {
    auto remaining_size = buffer_.size() - read_index_;
    std::copy(buffer_.data() + read_index_,
              buffer_.data() + read_index_ + remaining_size, data);
    std::copy(buffer_.data(), buffer_.data() + (read_size_ - remaining_size),
              data + remaining_size);
    read_index_ += step_size_;
    if (read_index_ > buffer_.size()) {
      read_index_ -= buffer_.size();
    }
  } else {
    // default read
    std::copy(buffer_.data() + read_index_,
              buffer_.data() + read_index_ + read_size_, data);
    read_index_ += step_size_;
  }
  available_data_size_ -= step_size_;

  return true;
}

}  // namespace rtff
