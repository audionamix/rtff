#ifndef RTFF_BUFFER_MULTICHANNEL_RING_BUFER_H_
#define RTFF_BUFFER_MULTICHANNEL_RING_BUFER_H_

#include <cstdint>

#include "rtff/buffer/ring_buffer.h"
#include "rtff/buffer/audio_buffer.h"

namespace rtff {

class MultichannelRingBuffer {
 public:
  MultichannelRingBuffer(uint32_t write_size, uint32_t read_size, uint32_t step_size,
                         uint8_t channel_count);
  
  void InitWithZeros(uint32_t frame_number);
  
  RingBuffer& operator[](uint8_t channel_idx);
  const RingBuffer& operator[](uint8_t channel_idx) const;

  void Write(const AudioBuffer& buffer);
  bool Read(AudioBuffer* buffer);
  void Write(const Buffer<float>& buffer);
  bool Read(Buffer<float>* buffer);

 private:
  std::vector<RingBuffer> buffers_;
};

}  // namespace rtff

#endif  // RTFF_BUFFER_MULTICHANNEL_RING_BUFER_H_
