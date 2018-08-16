#include "rtff/abstract_filter.h"

#include "rtff/buffer/buffer.h"
#include "rtff/filter_impl.h"
#include "rtff/buffer/ring_buffer.h"
#include "rtff/buffer/overlap_ring_buffer.h"

namespace rtff {

class AbstractFilter::Impl {
 public:
  TimeAmplitudeBuffer amplitude_block;
  TimeAmplitudeBuffer output_amplitude_block;
  TimeFrequencyBuffer frequential_block;
};

AbstractFilter::AbstractFilter()
    : fft_size_(2048), overlap_(2048 * 0.5), block_size_(512) {}

AbstractFilter::~AbstractFilter() {}

void AbstractFilter::Init(uint8_t channel_count, uint32_t fft_size,
                          uint32_t overlap, std::error_code& err) {
  fft_size_ = fft_size;
  overlap_ = overlap;
  Init(channel_count, err);
}

void AbstractFilter::Init(uint8_t channel_count, std::error_code& err) {
  channel_count_ = channel_count;
  InitBuffers();

  // init single block buffers
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

void AbstractFilter::InitBuffers() {
  input_buffer_ = std::make_shared<MultichannelOverlapRingBuffer>(
      fft_size(), hop_size(), channel_count());
  output_buffer_ = std::make_shared<MultichannelRingBuffer>(block_size() * 8,
                                                            channel_count());

  // initialize the intput_buffer_ with hop_size frames of zeros
  if (fft_size() > block_size()) {
    input_buffer_->InitWithZeros(fft_size() - block_size());
  }
}

void AbstractFilter::set_block_size(uint32_t value) {
  block_size_ = value;
  InitBuffers();
}
uint32_t AbstractFilter::block_size() const { return block_size_; }
uint8_t AbstractFilter::channel_count() const { return channel_count_; }

uint32_t AbstractFilter::window_size() const { return impl_->window_size(); }
uint32_t AbstractFilter::fft_size() const { return fft_size_; }
uint32_t AbstractFilter::overlap() const { return overlap_; }
uint32_t AbstractFilter::hop_size() const { return fft_size_ - overlap_; }

uint32_t AbstractFilter::FrameLatency() const {
  // latency has three different states:
  if (hop_size() % block_size() == 0) {
    // when hop size can be devided by block size
    return fft_size() - block_size();
  } else if (block_size() < fft_size()) {
    return fft_size();
  } else {
    return block_size();
  }
}

void AbstractFilter::ProcessBlock(AudioBuffer* buffer) {
  auto frame_count = buffer->frame_count();
  input_buffer_->Write(*buffer, frame_count);

  // process as many blocks as possible
  while (input_buffer_->Read(&(buffers_->amplitude_block))) {
    impl_->Analyze(buffers_->amplitude_block, &(buffers_->frequential_block));
    ProcessTransformedBlock(buffers_->frequential_block.data_ptr(),
                            buffers_->frequential_block.size());
    impl_->Synthesize(buffers_->frequential_block,
                      &(buffers_->output_amplitude_block));
    output_buffer_->Write(buffers_->output_amplitude_block,
                          buffers_->output_amplitude_block.size());
  }

  if (output_buffer_->Read(buffer, frame_count)) {
    return;
  }
  // if we don't have enough data to be read, just fill with zeros
  for (auto channel_idx = 0; channel_idx < buffer->channel_count(); channel_idx++) {
    std::fill(buffer->data(channel_idx), buffer->data(channel_idx) + frame_count, 0);
  }
}

}  // namespace rtff
