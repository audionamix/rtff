#ifndef RTFF_FFT_FFTW_FFTW_FFT_H_
#define RTFF_FFT_FFTW_FFTW_FFT_H_

#include <complex>
#include <memory>
#include <system_error>

#include "rtff/fft/fft.h"

namespace rtff {

/**
 * @brief the fftw3f Library implementation of the Fast Fourier Transform
 * computer
 */
class FFTWFft : public Fft {
 public:
  FFTWFft();
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

#endif  // RTFF_FFT_MKL_FFT_H_
