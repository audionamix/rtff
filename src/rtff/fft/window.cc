#include "rtff/fft/window.h"

#include <iostream>

namespace rtff {

Eigen::VectorXf Window::Make(Type type, uint32_t size) {
  // TODO(gvincke): move this to a constant definition file
  const static double pi = 3.14159265358979323846264338327950288419;

  Eigen::VectorXf window = Eigen::VectorXf::Zero(size);
  if (type == Type::Hamming) {
    auto alpha = 0.54;
    auto beta = 1 - alpha;
    for (uint32_t window_idx = 0; window_idx < window.size(); window_idx++) {
      window[window_idx] =
          alpha - beta * cos((2 * pi * window_idx) / (window.size() - 1));
    }
  } else if (type == Type::Blackman) {
    auto alpha = 0.42;
    auto beta = 0.5;
    auto gamma = 0.08;
    for (uint32_t window_idx = 0; window_idx < window.size(); window_idx++) {
      window[window_idx] =
          alpha - beta * cos((2 * pi * window_idx) / (window.size() - 1))
                + gamma * cos((4 * pi * window_idx) / (window.size() - 1));
    }
  } else {
    std::cerr << "Unkown window type" << std::endl;
    window = Eigen::VectorXf::Ones(size);
  }
  
  return window;
}

Eigen::VectorXf Window::MakeInverse(Type analysis_type, Type sythesis_type,
                                    uint32_t size, uint32_t step_size) {
  auto analysis = Make(analysis_type, size);
  auto synthesis = Make(sythesis_type, size);

  Eigen::VectorXf window = Eigen::VectorXf::Zero(size);

  // analysis windows
  for (auto end_idx = step_size; end_idx <= (2 * size) - step_size;
       end_idx += step_size) {
    if (end_idx < size) {
      // first half
      auto window_part =
          analysis.tail(end_idx).array() * synthesis.tail(end_idx).array();
      window.head(end_idx).array() += window_part;
    } else if (end_idx > size) {
      // second half
      auto window_part = analysis.head(size - (end_idx - size)).array() *
                         synthesis.head(size - (end_idx - size)).array();
      window.tail(size - (end_idx - size)).array() += window_part;
    } else {
      // center
      window.array() += analysis.array() * synthesis.array();
    }
  }

  return window;
}

}  // namespace rtff
