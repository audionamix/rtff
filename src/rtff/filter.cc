#include "rtff/filter.h"

namespace rtff {

Filter::Filter()
    : rtff::AbstractFilter(),
      execute([](std::vector<std::complex<float>*>, uint32_t) {}) {}

Filter::~Filter() {}
  
void Filter::ProcessTransformedBlock(std::vector<std::complex<float>*> data,
                                     uint32_t size) {
  execute(data, size);
}

}  // namespace rtff
