#ifndef RTFF_FFT_WINDOW_H_
#define RTFF_FFT_WINDOW_H_

#include <Eigen/Core>
#include <cstdint>

namespace rtff {

// see https://en.wikipedia.org/wiki/Window_function
// for information about window function
class Window {
 public:
  enum class Type : uint8_t { Hamming };

  static Eigen::VectorXf Make(Type type, uint32_t size);
  static Eigen::VectorXf MakeInverse(Type analysis_type, Type sythesis_type,
                                     uint32_t size, uint32_t step_size);
};
}  // namespace rtff

#endif  // RTFF_FFT_WINDOW_H_
