#ifndef RTFF_FFT_MKL_MKL_FFT_H_
#define RTFF_FFT_MKL_MKL_FFT_H_

#include "rtff/fft/fft.h"
#include "rtff/fft/mkl/mkl_fft_context.h"

namespace rtff {

// MKLFft a fast fourier computer base on the MKL Library
class MKLFft : public Fft {
 public:
  void Init(uint32_t size, std::error_code& err);
  void Forward(const float* real_data,
               std::complex<float>* complex_data) override;
  void Backward(const std::complex<float>* complex_data,
                float* real_data) override;

 private:
  MKLFftContext context_;
};
}  // namespace rtff

#endif  // RTFF_FFT_MKL_FFT_H_
