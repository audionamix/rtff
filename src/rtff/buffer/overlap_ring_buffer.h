#ifndef RTFF_BUFFER_OVERLAP_RING_BUFER_H_
#define RTFF_BUFFER_OVERLAP_RING_BUFER_H_

#include <cstdint>
#include <vector>

namespace rtff {

template <typename T>
class Buffer;
class AudioBuffer;

/**
 * OverlapRingBuffer represents a Ring buffer with an overlap concept at read
 * time.
 * For example, after reading N samples of indexes [1, 2 ... N], the read index
 * will be moved by a step size M that may be different to N (the read size)
 * So the next read samples will be [M, M+1, ... N, ..., N + M]
 */
class OverlapRingBuffer {
 public:
  OverlapRingBuffer(uint32_t read_size, uint32_t step_size);
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
};

/**
 * MultichannelOverlapRingBuffer is a multichannel wrapper around the
 * OverlapRingBuffer
 */
class MultichannelOverlapRingBuffer {
 public:
  MultichannelOverlapRingBuffer(uint32_t read_size, uint32_t step_size,
                                uint8_t channel_count);

  void InitWithZeros(uint32_t frame_number);

  OverlapRingBuffer& operator[](uint8_t channel_idx);
  const OverlapRingBuffer& operator[](uint8_t channel_idx) const;

  void Write(const AudioBuffer& buffer, uint32_t frame_count);
  bool Read(AudioBuffer* buffer);
  void Write(const Buffer<float>& buffer, uint32_t frame_count);
  bool Read(Buffer<float>* buffer);

 private:
  std::vector<OverlapRingBuffer> buffers_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_OVERLAP_RING_BUFER_H_
