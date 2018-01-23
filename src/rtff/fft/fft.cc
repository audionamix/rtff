#include "rtff/fft/fft.h"

#include "rtff/fft/mkl/mkl_fft.h"

namespace rtff {

std::shared_ptr<Fft> Fft::Create(uint32_t size, std::error_code& err) {
  auto fft = std::make_shared<MKLFft>();
  fft->Init(size, err);
  return fft;
}

}  // namespace rtff
