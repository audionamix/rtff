#ifndef RTFF_FFT_FFT_H_
#define RTFF_FFT_FFT_H_

#include <complex>
#include <cstdint>
#include <memory>
#include <system_error>

namespace rtff {

/**
 * @brief base class for Fast fourier transform computers
 */
class Fft {
 public:
  /**
   * @brief Create a default computer based on various libraries depending on
   * your system
   * @param size: the size in samples of the fft
   * @param err: an error code that gets set if something goes wrong
   */
  static std::shared_ptr<Fft> Create(uint32_t size, std::error_code& err);

  /**
   * @brief transform a buffer of real signal data to its time frequency
   * complex representation
   * @param real_data: the signal data
   * @param complex_data: the fourier transform of the real data
   */
  virtual void Forward(const float* real_data,
                       std::complex<float>* complex_data) = 0;
  /**
   * @brief transform the complex time frequency representation back to the
   * time domain
   * @param complex_data: the time frequency data
   * @param real_data: the inverse fourier transform of the complex_data
   */
  virtual void Backward(const std::complex<float>* complex_data,
                        float* real_data) = 0;
};

}  // namespace rtff

#endif  // RTFF_FFT_FFT_H_
