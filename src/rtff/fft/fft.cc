#include "rtff/fft/fft.h"

// --------
// Use FFTW if defined
#ifdef RTFF_USE_FFTW

#include "rtff/fft/fftw/fftw_fft.h"
using FFTType = rtff::FFTWFft;

#else // RTFF_USE_FFTW

// If fftw not defined and mkl is, use it
#ifdef RTFF_USE_MKL

#include "rtff/fft/mkl/mkl_fft.h"
using FFTType = rtff::MKLFft;

// If none defined, use eigen
#else // RTFF_USE_MKL

#include "rtff/fft/eigen/eigen_fft.h"
using FFTType = rtff::EigenFft;

#endif  // RTFF_USE_MKL
#endif  // RTFF_USE_FFTW
// --------

namespace rtff {

std::shared_ptr<Fft> Fft::Create(uint32_t size, std::error_code& err) {
  auto fft = std::make_shared<FFTType>();
  fft->Init(size, err);
  return fft;
}

}  // namespace rtff
