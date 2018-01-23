#ifndef RTFF_FFT_MKL_MKL_FFT_CONTEXT_H_
#define RTFF_FFT_MKL_MKL_FFT_CONTEXT_H_

#include <system_error>
#include <memory>
#include <map>
#include <string>

#include <mkl.h>

namespace rtff {

namespace mkl {

class ErrorCategory : public std::error_category {
 public:
  const char* name() const throw() override;
  std::string message(int ev) const override;
  bool equivalent(const std::error_code& code, int condition) const
      throw() override;
};
std::error_code make_error_code(MKL_LONG error);
std::error_condition make_error_condition(MKL_LONG error);
std::error_code make_error(MKL_LONG status);
}  // namespace mkl

class MKLFftContext {
 public:
  MKLFftContext();
  ~MKLFftContext();
  void Init(uint32_t size, std::error_code& err);
  uint32_t size() const;
  DFTI_DESCRIPTOR_HANDLE descriptor();

 private:
  void InitDescriptor(std::error_code& err);
  void set_descriptor(std::map<DFTI_CONFIG_PARAM, DFTI_CONFIG_VALUE> settings,
                      std::error_code& err);

  bool initialized_;
  uint32_t size_;
  DFTI_DESCRIPTOR_HANDLE descriptor_;
};

}  // namespace rtff

#endif  // RTFF_FFT_MKL_MKL_FFT_CONTEXT_H_
