#ifndef RTFF_BUFFER_AUDIO_BUFFER_H_
#define RTFF_BUFFER_AUDIO_BUFFER_H_

#include <cstdint>
#include <vector>

namespace rtff {

// AudioBuffer is an interleaved audio signal data buffer.
class AudioBuffer {
 public:
  void Init(uint32_t frame_count, uint8_t channel_count);
  float* data();
  const float* data() const;
  uint32_t frame_count() const;
  uint8_t channel_count() const;

 private:
  uint32_t frame_count_;
  uint32_t channel_count_;
  std::vector<float> data_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_AUDIO_BUFFER_H_
