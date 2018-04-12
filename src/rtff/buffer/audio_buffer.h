#ifndef RTFF_BUFFER_AUDIO_BUFFER_H_
#define RTFF_BUFFER_AUDIO_BUFFER_H_

#include <cstdint>
#include <vector>

namespace rtff {

// AudioBuffer is an interleaved audio signal data buffer.
class AudioBuffer {
 public:
  AudioBuffer(uint32_t frame_count, uint8_t channel_count);

  void fromInterleaved(const float* data);
  void toInterleaved(float* data) const;

  float* data(uint8_t channel_idx);
  const float* data(uint8_t channel_idx) const;

  uint32_t frame_count() const;
  uint8_t channel_count() const;

 private:
  std::vector<std::vector<float>> data_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_AUDIO_BUFFER_H_
