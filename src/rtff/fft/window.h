#ifndef RTFF_FFT_WINDOW_H_
#define RTFF_FFT_WINDOW_H_

#include <cstdint>
#include <Eigen/Core>

namespace rtff {
enum class WindowType : uint8_t { Hamming };

class Window {
 public:
  static Eigen::VectorXf Make(WindowType type, uint32_t size);
  static Eigen::VectorXf MakeInverse(WindowType analysis_type,
                                     WindowType sythesis_type, uint32_t size,
                                     uint32_t step_size);
};
}  // namespace spex

#endif  // RTFF_FFT_WINDOW_H_
