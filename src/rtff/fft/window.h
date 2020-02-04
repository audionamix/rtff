#ifndef RTFF_FFT_WINDOW_H_
#define RTFF_FFT_WINDOW_H_

#include <Eigen/Core>
#include <cstdint>
#include "rtff/fft/window_type.h"

namespace rtff {

/**
 * @brief Utility function to build an analysis window
 * @see https://en.wikipedia.org/wiki/Window_function for more
 */
class Window {
 public:

  /**
   * @brief generate an analysis window
   * @param type: the type of the window
   * @param size: the window size in sample
   */
  static Eigen::VectorXf Make(fft_window::Type type, uint32_t size);
  /**
   * @brief generate an unwindow used in the inverse fft step
   * @param analysis_type: the type of the window used in the analysis step
   * @param sythesis_type: the type of the window used in the synthesis step
   * @param size: the window size in sample
   * @param step_size: the hop size used by the filter
   */
  static Eigen::VectorXf MakeInverse(fft_window::Type analysis_type,
                                     fft_window::Type sythesis_type,
                                     uint32_t size, uint32_t step_size);
};
}  // namespace rtff

#endif  // RTFF_FFT_WINDOW_H_
