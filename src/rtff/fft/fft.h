#ifndef RTFF_FFT_FFT_H_
#define RTFF_FFT_FFT_H_

#include <complex>
#include <cstdint>
#include <memory>

namespace rtff {

class Fft {
 public:
  static std::shared_ptr<Fft> Create(uint32_t size, std::error_code& err);
  virtual void Forward(const float* real_data,
                       std::complex<float>* complex_data) = 0;
  virtual void Backward(const std::complex<float>* complex_data,
                        float* real_data) = 0;
};

}  // namespace rtff

#endif  // RTFF_FFT_FFT_H_
