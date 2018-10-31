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

/**
 * @brief the Filter Implementation.
 * That class gather most of the filter implementation details.
 * It is where the time amplitude signal is converted into a time frequency one
 */
class FilterImpl {
 public:
  /**
   * @brief Initialize
   * @param fft_size: the length in samples of the fourier transform window.
   * @param overlap: the number of samples that will be kept between each
   * @param channel_count: the number of channel of the input signal
   * @param err: an error code that gets set if something goes wrong
   */
  void Init(uint32_t fft_size, uint32_t overlap, uint8_t channel_count,
            std::error_code& err);

  /**
   * @brief convert a signal to its time frequency representation
   * @param amplitude: the original signal buffer
   * @param frequential: the time frequency representation
   * @note this function modifies the original signal by multiplying it with
   * the analysis window
   */
  void Analyze(TimeAmplitudeBuffer& amplitude,
               TimeFrequencyBuffer* frequential);

  /**
   * @brief convert a time frequency representation into its signal
   * @param frequential: the time frequency representation
   * @param amplitude: the signal buffer
   */
  void Synthesize(const TimeFrequencyBuffer& frequential,
                  TimeAmplitudeBuffer* amplitude);

  /**
   * @return the window used for the analysis stage
   */
  const Eigen::VectorXf& analysis_window() const;
  /**
   * @return the window used for the synthesis stage
   */
  const Eigen::VectorXf& synthesis_window() const;
  /**
   * @return the overlap in samples
   */
  uint32_t overlap() const;
  /**
   * @return the fft size in samples
   */
  uint32_t fft_size() const;
  /**
   * @return the window size in samples
   */
  uint32_t window_size() const;
  /**
   * @return the hop size in sample
   */
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
