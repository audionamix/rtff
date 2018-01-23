#include "rtff/fft/mkl/mkl_fft.h"

namespace rtff {

void MKLFft::Init(uint32_t size, std::error_code& err) {
  context_.Init(size, err);
}

void MKLFft::Forward(const float* real_data,
                     std::complex<float>* complex_data) {
  auto interleaved_complex_array = reinterpret_cast<float*>(complex_data);
  DftiComputeForward(context_.descriptor(), (float*)(real_data),
                     interleaved_complex_array);
}

void MKLFft::Backward(const std::complex<float>* complex_data,
                      float* real_data) {
  DftiComputeBackward(context_.descriptor(), (float*)(complex_data),
                      real_data);
}

}  // namespace rtff
