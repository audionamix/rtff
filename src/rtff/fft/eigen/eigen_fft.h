#ifndef RTFF_FFT_EIGEN_EIGEN_FFT_H_
#define RTFF_FFT_EIGEN_EIGEN_FFT_H_

#include <complex>
#include <memory>
#include <system_error>

#include "rtff/fft/fft.h"

namespace rtff {

class EigenFft : public Fft {
 public:
  EigenFft();
  void Init(uint32_t size, std::error_code& err);
  void Forward(const float* real_data,
               std::complex<float>* complex_data) override;
  void Backward(const std::complex<float>* complex_data,
                float* real_data) override;

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace rtff

#endif  // RTFF_FFT_EIGEN_EIGEN_FFT_H_
