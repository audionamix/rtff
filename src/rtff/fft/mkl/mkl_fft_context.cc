#include "rtff/fft/mkl/mkl_fft_context.h"

namespace rtff {

namespace mkl {

const char* ErrorCategory::name() const throw() { return "MKL error"; }
std::string ErrorCategory::message(int ev) const {
  return DftiErrorMessage(ev);
}
bool ErrorCategory::equivalent(const std::error_code& code, int condition) const
    throw() {
  return *this == code.category() &&
         static_cast<int>(default_error_condition(code.value()).value()) ==
             condition;
}
std::error_code make_error_code(MKL_LONG error) {
  ErrorCategory category;
  return std::error_code(static_cast<int>(error), category);
}
std::error_condition make_error_condition(MKL_LONG error) {
  ErrorCategory category;
  return std::error_condition(static_cast<int>(error), category);
}
std::error_code make_error(MKL_LONG status) {
  std::error_code err;
  if (status && !DftiErrorClass(status, DFTI_NO_ERROR)) {
    err = make_error_code(status);
  }
  return err;
}

}  // namespace mkl

MKLFftContext::MKLFftContext() : initialized_(false) {}
MKLFftContext::~MKLFftContext() {
  if (initialized_) {
    DftiFreeDescriptor(&descriptor_);
  }
}
void MKLFftContext::Init(uint32_t size, std::error_code& err) {
  if (initialized_) {
    DftiFreeDescriptor(&descriptor_);
  }
  size_ = size;
  InitDescriptor(err);
}

uint32_t MKLFftContext::size() const { return size_; }
DFTI_DESCRIPTOR_HANDLE MKLFftContext::descriptor() { return descriptor_; }

void MKLFftContext::InitDescriptor(std::error_code& err) {
  err = mkl::make_error(
      DftiCreateDescriptor(&descriptor_, DFTI_SINGLE, DFTI_REAL, 1, size_));
  if (err) {
    return;
  }

  std::map<DFTI_CONFIG_PARAM, DFTI_CONFIG_VALUE> descriptor;
  descriptor[DFTI_PLACEMENT] = DFTI_NOT_INPLACE;
  descriptor[DFTI_CONJUGATE_EVEN_STORAGE] = DFTI_COMPLEX_COMPLEX;
  descriptor[DFTI_PACKED_FORMAT] = DFTI_CCS_FORMAT;
  set_descriptor(descriptor, err);
  if (err) {
    return;
  }

  //  // vDSP style in order to have the same basseline for tests.
  //  auto forward_scaling_factor = 2.f;
  auto forward_scaling_factor = 1.f;
  auto backward_scaling_factor = 1.f / (forward_scaling_factor * size_);
  err = mkl::make_error(
      DftiSetValue(descriptor_, DFTI_FORWARD_SCALE, forward_scaling_factor));
  if (err) {
    return;
  }
  err = mkl::make_error(
      DftiSetValue(descriptor_, DFTI_BACKWARD_SCALE, backward_scaling_factor));
  if (err) {
    return;
  }

  err = mkl::make_error(DftiCommitDescriptor(descriptor_));
}

void MKLFftContext::set_descriptor(
    std::map<DFTI_CONFIG_PARAM, DFTI_CONFIG_VALUE> settings,
    std::error_code& err) {
  for (const auto& setting : settings) {
    const auto config_parameter = setting.first;
    const auto config_value = setting.second;
    err = mkl::make_error(
        DftiSetValue(descriptor_, config_parameter, config_value));
    if (err) {
      return;
    }
  }
}

}  // namespace rtff
