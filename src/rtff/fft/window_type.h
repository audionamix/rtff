#ifndef RTFF_FFT_WINDOW_TYPE_H_
#define RTFF_FFT_WINDOW_TYPE_H_

#include <cstdint>

namespace rtff {
namespace fft_window {
  /**
   * @brief enumerate representing the analysis window type.
   */
  enum class Type : uint8_t { Hamming, Blackman, Hann };
} // namespace fft_window
} // namespace rtff

#endif // RTFF_FFT_WINDOW_TYPE_H_
