#ifndef RTFF_FILTER_IMPL_H_
#define RTFF_FILTER_IMPL_H_

#include <memory>
#include <system_error>
#include <vector>

#include <Eigen/Core>

#include "rtff/buffer/audio_buffer.h"
#include "rtff/buffer/buffer.h"
#include "rtff/buffer/ring_buffer.h"

namespace rtff {

class Fft;

class FilterImpl {
 public:
  void Init(uint32_t fft_size, uint32_t overlap, uint8_t channel_count,
            std::error_code& err);

  void Analyze(TimeAmplitudeBuffer& amplitude,
               TimeFrequencyBuffer* frequential);

  void Synthesize(const TimeFrequencyBuffer& frequential,
                  TimeAmplitudeBuffer* amplitude);

  const Eigen::VectorXf& analysis_window() const;
  const Eigen::VectorXf& synthesis_window() const;
  uint32_t overlap() const;
  uint32_t fft_size() const;
  uint32_t window_size() const;
  uint32_t hop_size() const;

 private:
  uint32_t fft_size_, overlap_;

  Eigen::VectorXf analysis_window_;
  Eigen::VectorXf synthesis_window_;
  Eigen::VectorXf unwindow_;

  std::shared_ptr<Fft> fft_;

  std::vector<Eigen::VectorXf> previous_buffer_;
  std::vector<Eigen::VectorXf> result_buffer_;
  std::vector<Eigen::VectorXf> post_ifft_buffer_;
};

}  // namespace rtff

#endif  // RTFF_FILTER_IMPL_H_
