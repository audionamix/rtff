#include "rtff/fft/fft.h"

// --------
// Switch the FFT type according to compile options
#ifndef RTFF_USE_MKL

#include "rtff/fft/fftw/fftw_fft.h"
using FFTType = rtff::FFTWFft;

#else  // RTFF_USE_MKL

#include "rtff/fft/mkl/mkl_fft.h"
using FFTType = rtff::MKLFft;

#endif  // RTFF_USE_MKL
// --------

namespace rtff {

std::shared_ptr<Fft> Fft::Create(uint32_t size, std::error_code& err) {
  auto fft = std::make_shared<FFTType>();
  fft->Init(size, err);
  return fft;
}

}  // namespace rtff

