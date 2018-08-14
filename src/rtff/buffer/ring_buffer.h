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
  RingBuffer(uint32_t container_size);
  void InitWithZeros(uint32_t count);
  void Write(const float* data, uint32_t frame_count);
  bool Read(float* data, uint32_t frame_count);

 private:
  uint32_t write_index_;
  uint32_t read_index_;
  uint32_t available_data_size_;
  std::vector<float> buffer_;
};

/**
 * MultichannelRingBuffer is a multichannel wrapper around the RingBuffer
 */
class MultichannelRingBuffer {
 public:
  MultichannelRingBuffer(uint32_t container_size, uint8_t channel_count);

  void InitWithZeros(uint32_t frame_number);

  RingBuffer& operator[](uint8_t channel_idx);
  const RingBuffer& operator[](uint8_t channel_idx) const;

  void Write(const AudioBuffer& buffer, uint32_t frame_count);
  bool Read(AudioBuffer* buffer, uint32_t frame_count);
  void Write(const Buffer<float>& buffer, uint32_t frame_count);
  bool Read(Buffer<float>* buffer, uint32_t frame_count);

 private:
  std::vector<RingBuffer> buffers_;
};
}  // namespace rtff

#endif  // RTFF_BUFFER_RING_BUFER_H_
