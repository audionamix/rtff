#ifndef RTFF_BUFFER_AUDIO_BUFFER_H_
#define RTFF_BUFFER_AUDIO_BUFFER_H_

#include <cstdint>
#include <vector>

namespace rtff {

/**
 * @brief a fixed size buffer of raw audio signal data
 */
class AudioBuffer {
 public:
  /**
   * @brief Constructor
   * @param frame_count: the number of samples of each channel
   * @param channel_count: the number of channels
   */
  AudioBuffer(uint32_t frame_count, uint8_t channel_count);

  /**
   * @brief fill the buffer with interleaved data
   * @param data: the raw audio data
   */
  void fromInterleaved(const float* data);

  /**
   * @brief fill interleaved audio data with the content of the buffer
   * @param data: the interleaved audio data
   */
  void toInterleaved(float* data) const;

  /**
   * @param channel_idx: the channel index
   * @return the pointer to deinterleaved audio data
   */
  float* data(uint8_t channel_idx);
  /**
   * @param channel_idx: the channel index
   * @return the pointer to deinterleaved audio data
   */
  const float* data(uint8_t channel_idx) const;

  /**
   * @return the number of samples contained in each channel
   */
  uint32_t frame_count() const;
  /**
   * @return the number of channels
   */
  uint8_t channel_count() const;

 private:
  std::vector<std::vector<float>> data_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_AUDIO_BUFFER_H_
