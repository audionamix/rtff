#ifndef RTFF_BUFFER_BUFFER_H_
#define RTFF_BUFFER_BUFFER_H_

#include <Eigen/Core>

namespace rtff {

/**
 * @brief A multichannel data storage class
 */
template <typename T>
class Buffer {
 public:
  using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
  /**
   * @brief Initialize and allocate memory
   * @param frame_count: the number of samples of each channel
   * @param channel_count: the number of channels
   */
  void Init(uint32_t frame_count, uint8_t channel_count) {
    data_.clear();
    for (uint8_t channel_idx = 0; channel_idx < channel_count; channel_idx++) {
      data_.push_back(Vector::Zero(frame_count));
    }
  }

  /**
   * @param channel_idx: the channel index
   * @return a reference to the raw data stored in a vector
   */
  Vector& channel(uint8_t channel_idx) { return data_[channel_idx]; }
  const Vector& channel(uint8_t channel_idx) const {
    return data_[channel_idx];
  }

  /**
   * @return the number of channels
   */
  uint8_t channel_count() const { return data_.size(); }

  /**
   * @return a vector of pointers giving access to raw data
   */
  std::vector<T*> data_ptr() {
    std::vector<T*> result;
    for (auto& vector : data_) {
      result.push_back(vector.data());
    }
    return result;
  }

  /**
   * @return the number of samples contained in each channel
   */
  uint32_t size() const { return data_[0].size(); }

 private:
  std::vector<Vector> data_;
};

using TimeAmplitudeBuffer = Buffer<float>;
using TimeFrequencyBuffer = Buffer<std::complex<float>>;

}  // namespace rtff

#endif  // RTFF_BUFFER_BUFFER_H_
