#include "rtff/buffer/audio_buffer.h"

namespace rtff {

void AudioBuffer::Init(uint32_t frame_count, uint8_t channel_count) {
  frame_count_ = frame_count;
  channel_count_ = channel_count;
  data_.resize(frame_count * channel_count);
}

float* AudioBuffer::data() { return data_.data(); }

const float* AudioBuffer::data() const { return data_.data(); }

uint32_t AudioBuffer::frame_count() const { return frame_count_; }

uint8_t AudioBuffer::channel_count() const { return channel_count_; }

}  // namespace rtff
