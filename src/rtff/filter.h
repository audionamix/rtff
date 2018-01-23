#ifndef RTFF_FILTER_H_
#define RTFF_FILTER_H_

#include <vector>
#include <memory>
#include <system_error>

#include <Eigen/Core>

#include "rtff/buffer/audio_buffer.h"
#include "rtff/buffer/ring_buffer.h"
#include "rtff/buffer/buffer.h"

namespace rtff {

class Fft;

class Filter {
 public:
  Filter();
  void Init(uint32_t block_size, uint8_t channel_count, std::error_code& err);
  void ProcessBlock(AudioBuffer* buffer);
  uint32_t FrameLatency() const;

  uint32_t window_size() const;
  Eigen::VectorXf analysis_window() const;
  Eigen::VectorXf synthesis_window() const;
  uint32_t fft_size() const;
  uint32_t overlap() const;
  uint32_t hop_size() const;

 protected:
  virtual void ProcessFreqBlock(FrequentialBuffer* buffer) = 0;

 private:
  void Analyze(AmplitudeBuffer& amplitude, FrequentialBuffer* frequential);
  void Synthesize(const FrequentialBuffer& frequential,
                  AmplitudeBuffer* amplitude);

  uint32_t fft_size_;
  uint32_t overlap_;
  RingBuffer input_buffer_, output_buffer_;
  AmplitudeBuffer amplitude_block_buffer_;
  AmplitudeBuffer output_amplitude_block_buffer_;
  FrequentialBuffer frequential_block_buffer_;

  Eigen::VectorXf analysis_window_;
  Eigen::VectorXf synthesis_window_;
  Eigen::VectorXf unwindow_;
  
  std::shared_ptr<Fft> fft_;
  
  std::vector<Eigen::VectorXf> previous_buffer_;
  std::vector<Eigen::VectorXf> result_buffer_;
  std::vector<Eigen::VectorXf> post_ifft_buffer_;
};

}  // namespace rtff

#endif  // RTFF_FILTER_H_
