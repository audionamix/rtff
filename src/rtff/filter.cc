#include "rtff/filter.h"

#include "rtff/buffer/buffer.h"
#include "rtff/filter_impl.h"

namespace rtff {

class Filter::Impl {
 public:
  TimeAmplitudeBuffer amplitude_block;
  TimeAmplitudeBuffer output_amplitude_block;
  TimeFrequencyBuffer frequential_block;
};

Filter::Filter() : fft_size_(2048), overlap_(1024) {}

void Filter::Init(uint32_t block_size, uint8_t channel_count,
                  std::error_code& err) {
  input_buffer_.Init(block_size, fft_size(), hop_size(), channel_count);
  output_buffer_.Init(hop_size(), block_size, block_size, channel_count);

  // init single block buffer
  buffers_ = std::make_shared<Impl>();
  buffers_->amplitude_block.Init(fft_size(), channel_count);
  buffers_->output_amplitude_block.Init(hop_size(), channel_count);
  buffers_->frequential_block.Init(fft_size() / 2 + 1, channel_count);

  impl_ = std::make_shared<FilterImpl>();
  impl_->Init(fft_size(), overlap(), channel_count, err);
  if (err) {
    return;
  }
}

uint32_t Filter::window_size() const { return impl_->window_size(); }
uint32_t Filter::fft_size() const { return fft_size_; }
uint32_t Filter::overlap() const { return overlap_; }
uint32_t Filter::hop_size() const { return fft_size_ - overlap_; }

uint32_t Filter::FrameLatency() const { return 0; }

void Filter::ProcessBlock(AudioBuffer* buffer) {
  input_buffer_.Write(*buffer);

  // process as many blocks as possible
  while (input_buffer_.Read(&(buffers_->amplitude_block))) {
    impl_->Analyze(buffers_->amplitude_block, &(buffers_->frequential_block));
    ProcessTransformedBlock(buffers_->frequential_block.data_ptr(),
                            buffers_->frequential_block.size());
    impl_->Synthesize(buffers_->frequential_block,
                      &(buffers_->output_amplitude_block));

    output_buffer_.Write(buffers_->output_amplitude_block);
  }

  if (output_buffer_.Read(buffer->data())) {
    return;
  }
  // if we don't have enough data to be read, just fill with zeros
  std::fill(buffer->data(),
            buffer->data() + buffer->channel_count() * buffer->frame_count(),
            0);
}

}  // namespace rtff
