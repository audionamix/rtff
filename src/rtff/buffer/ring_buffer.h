#ifndef RTFF_BUFFER_RING_BUFER_H_
#define RTFF_BUFFER_RING_BUFER_H_

#include <cstdint>
#include <vector>

namespace rtff {

template <typename T>
class Buffer;
class AudioBuffer;

/**
 * RingBuffer represent a circular buffer (see
 * https://en.wikipedia.org/wiki/Circular_buffer ) with potentially different
 * read an write size.
 * It is used to store enough data before starting a process without having to
 * allocate memory dynamically
 */
class RingBuffer {
 public:
  RingBuffer(uint32_t read_size, uint32_t step_size);
  void InitWithZeros(uint32_t count);
  void Write(const float* data, uint32_t frame_count);
  bool Read(float* data);

 private:
  uint32_t read_size_;
  uint32_t step_size_;

  uint32_t write_index_;
  uint32_t read_index_;
  uint32_t available_data_size_;
  std::vector<float> buffer_;

  // temp data to avoid allocating memory after Init
  std::vector<float> temp_read_data_;
  std::vector<float> temp_write_data_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_RING_BUFER_H_
