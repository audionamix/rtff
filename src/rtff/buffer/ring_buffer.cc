#include "rtff/buffer/ring_buffer.h"

#include "rtff/buffer/audio_buffer.h"
#include "rtff/buffer/buffer.h"

namespace rtff {

RingBuffer::RingBuffer(uint32_t write_size, uint32_t read_size,
                       uint32_t step_size) {
  write_size_ = write_size;
  read_size_ = read_size;
  step_size_ = step_size;
  write_index_ = 0;
  read_index_ = 0;
  available_data_size_ = 0;

  buffer_.resize(std::max(read_size_, write_size_) * 2);
  // used in RingBuffer::Read(AmplitudeBuffer* buffer)
  temp_read_data_.resize(read_size);
  // used in RingBuffer::Write(const AmplitudeBuffer& buffer)
  temp_write_data_.resize(write_size);
}

void RingBuffer::InitWithZeros(uint32_t count) {
  if (write_index_ + count > buffer_.size()) {
    auto remaining_size = buffer_.size() - write_index_;
    std::fill(buffer_.data() + write_index_,
              buffer_.data() + write_index_ + remaining_size, 0);
    std::fill(buffer_.data(), buffer_.data() + (count - remaining_size), 0);
    write_index_ = (write_size_ - remaining_size);
  } else {
    std::fill(buffer_.data() + write_index_,
              buffer_.data() + write_index_ + count, 0);
    write_index_ += count;
  }
  available_data_size_ += count;
}

void RingBuffer::Write(const float* data) {
  if (write_index_ + write_size_ > buffer_.size()) {
    // When we reach the end of the buffer
    auto remaining_size = buffer_.size() - write_index_;
    std::copy(data, data + remaining_size, buffer_.data() + write_index_);
    std::copy(data + remaining_size, data + write_size_, buffer_.data());
    write_index_ = (write_size_ - remaining_size);
  } else {
    // we have enough size remaining
    std::copy(data, data + write_size_, buffer_.data() + write_index_);
    write_index_ += write_size_;
  }
  available_data_size_ += write_size_;
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
