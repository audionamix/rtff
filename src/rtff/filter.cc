#include "rtff/filter.h"

#include "rtff/fft/fft.h"
#include "rtff/fft/window.h"

namespace rtff {

Filter::Filter() : fft_size_(2048), overlap_(1024) {}

void Filter::Init(uint32_t block_size, uint8_t channel_count,
                  std::error_code& err) {
  input_buffer_.Init(block_size, fft_size(), hop_size(), channel_count);
  output_buffer_.Init(hop_size(), block_size, block_size, channel_count);

  // init single block buffer
  amplitude_block_buffer_.Init(fft_size(), channel_count);
  output_amplitude_block_buffer_.Init(hop_size(), channel_count);
  frequential_block_buffer_.Init(fft_size() / 2 + 1, channel_count);

  // init the window to an hamming
  analysis_window_ = Window::Make(Window::Type::Hamming, fft_size_);
  synthesis_window_ = Window::Make(Window::Type::Hamming, fft_size_);
  unwindow_ = Window::MakeInverse(Window::Type::Hamming, Window::Type::Hamming,
                                  fft_size_, hop_size());

  // init the fft
  fft_ = Fft::Create(fft_size_, err);
  if (err) {
    return;
  }

  // init inverse transform temp data
  previous_buffer_.resize(channel_count);
  result_buffer_.resize(channel_count);
  post_ifft_buffer_.resize(channel_count);
  for (auto channel_idx = 0; channel_idx < channel_count; channel_idx++) {
    previous_buffer_[channel_idx] =
        Eigen::VectorXf::Zero(analysis_window().size());
    post_ifft_buffer_[channel_idx].resize(analysis_window().size());
    result_buffer_[channel_idx].resize(analysis_window().size() + hop_size());
  }
}

const Eigen::VectorXf& Filter::analysis_window() const {
  return analysis_window_;
}
const Eigen::VectorXf& Filter::synthesis_window() const {
  return synthesis_window_;
}
uint32_t Filter::fft_size() const { return fft_size_; }
uint32_t Filter::overlap() const { return overlap_; }
uint32_t Filter::hop_size() const { return fft_size_ - overlap_; }

uint32_t Filter::FrameLatency() const { return 0; }

void Filter::ProcessBlock(AudioBuffer* buffer) {
  input_buffer_.Write(*buffer);

  // process as many blocks as possible
  while (input_buffer_.Read(&amplitude_block_buffer_)) {
    Analyze(amplitude_block_buffer_, &frequential_block_buffer_);
    ProcessTransformedBlock(&frequential_block_buffer_);
    Synthesize(frequential_block_buffer_, &output_amplitude_block_buffer_);

    output_buffer_.Write(output_amplitude_block_buffer_);
  }

  if (output_buffer_.Read(buffer->data())) {
    return;
  }
  // if we don't have enough data to be read, just fill with zeros
  memset(buffer->data(), 0,
         buffer->channel_count() * buffer->frame_count() * sizeof(float));
}

void Filter::Analyze(TimeAmplitudeBuffer& amplitude,
                     TimeFrequencyBuffer* frequential) {
  for (uint8_t channel_idx = 0; channel_idx < amplitude.channel_count();
       channel_idx++) {
    // apply the analysis window
    amplitude.channel(channel_idx).array() *= analysis_window_.array();
    // compute the fft and store it into the frequential buffer
    fft_->Forward(amplitude.channel(channel_idx).data(),
                  frequential->channel(channel_idx).data());
  }
}

void Filter::Synthesize(const TimeFrequencyBuffer& frequential,
                        TimeAmplitudeBuffer* amplitude) {
  for (uint8_t channel_idx = 0; channel_idx < frequential.channel_count();
       channel_idx++) {
    auto& result_ = result_buffer_[channel_idx];
    auto& previous_ = previous_buffer_[channel_idx];
    auto& post_ifft = post_ifft_buffer_[channel_idx];

    // ifft
    fft_->Backward(frequential.channel(channel_idx).data(), post_ifft.data());
    // apply synthesis window and sum to previous data
    // sum with previous data
    memset(result_.data(), 0, result_.size() * sizeof(float));

    result_.head(analysis_window().size()) = previous_;
    result_.tail(analysis_window().size()).array() +=
        post_ifft.array() * synthesis_window_.array() / unwindow_.array();

    // keep previous buffer for synthesis
    previous_ = result_.tail(analysis_window().size());

    // unwindow to get the right buffer
    amplitude->channel(channel_idx).noalias() =
        result_.head(hop_size()).transpose();
  }
}

}  // namespace rtff
