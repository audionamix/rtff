#include "rtff/fft/eigen/eigen_fft.h"

#include <unsupported/Eigen/FFT>

namespace rtff {

class EigenFft::Impl {
public:
  Impl() : fft() {
    fft.SetFlag(Eigen::FFT<float>::Flag::HalfSpectrum);
  }
  Eigen::FFT<float> fft;
  std::vector<float> timevec;
  std::vector<std::complex<float>> freqvec;
};

EigenFft::EigenFft() : impl_(std::make_shared<EigenFft::Impl>()) {}

void EigenFft::Init(uint32_t size, std::error_code& err) {
  // Initialize by running the fft and ifft once
  impl_->timevec.resize(size);
  impl_->freqvec.resize(size / 2);
  
  impl_->fft.inv(impl_->timevec, impl_->freqvec);
  impl_->fft.fwd(impl_->freqvec, impl_->timevec);
  
}

void EigenFft::Forward(const float* real_data,
                       std::complex<float>* complex_data) {
  std::copy(real_data, real_data + impl_->timevec.size(), std::begin(impl_->timevec));
  impl_->fft.fwd(impl_->freqvec, impl_->timevec);
  std::copy(std::begin(impl_->freqvec), std::end(impl_->freqvec), complex_data);
}

void EigenFft::Backward(const std::complex<float>* complex_data,
                        float* real_data) {
  std::copy(complex_data, complex_data + impl_->freqvec.size(), std::begin(impl_->freqvec));
  impl_->fft.inv(impl_->timevec, impl_->freqvec);
  std::copy(std::begin(impl_->timevec), std::end(impl_->timevec), real_data);
}

}  // namespace rtff
