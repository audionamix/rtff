#ifndef RTFF_BUFFER_RING_BUFER_H_
#define RTFF_BUFFER_RING_BUFER_H_

#include <cstdint>
#include <vector>

namespace rtff {

template <typename T>
class Buffer;
using AmplitudeBuffer = Buffer<float>;
class AudioBuffer;

class RingBuffer {
 public:
  void Init(uint32_t write_size, uint32_t read_size, uint32_t step_size,
            uint8_t channel_count);
  void Write(const float* data);
  void Write(const AudioBuffer& buffer);
  void Write(const AmplitudeBuffer& buffer);

  bool Read(float* data);
  bool Read(AmplitudeBuffer* buffer);

 private:
  uint32_t write_size_;
  uint32_t read_size_;
  uint32_t step_size_;

  uint32_t write_index_;
  uint32_t read_index_;
  uint32_t available_data_size_;
  uint8_t channel_count_;
  std::vector<float> buffer_;

  // temp data to avoid allocating memory after Init
  std::vector<float> temp_read_data_;
  std::vector<float> temp_write_data_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_RING_BUFER_H_
