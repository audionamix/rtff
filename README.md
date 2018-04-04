[![Build status](https://ci.appveyor.com/api/projects/status/5rlqnh3v74n606sw?svg=true)](https://ci.appveyor.com/project/Audionamix/rtff)

# Real Time Frequential Filtering

A framework to design audio filters in the time frequency domain.

## Example

```cpp
#include <iostream>

#include <Eigen/Core>

#include "rtff/filter.h"

...
rtff::AudioBuffer buffer;
buffer.Init(filter.block_size(), channel_number);

std::error_code err;
rtff::Filter filter;
filter.Init(channel_number, err);
if (err) {
  std::cerr << "Error when initializing the filter" << std::endl;
  return -1;
}
filter.execute = [](std::vector<std::complex<float>*> data, uint32_t size) {
  for (auto channel_idx = 0; channel_idx < data.size(); channel_idx++) {
    auto buffer = Eigen::Map<Eigen::VectorXcf>(data[channel_idx], size);

    // Do stuff to your buffer here !

  }
}
...
filter.ProcessBlock(&buffer);
```

## Latency

Computing the short time fourier transform implies a latency. If you want to
develop a plug-in, you need to inform the DAW about the number of frames of
latency produced by your filter.  
The `AbstractFilter::FrameLatency()` function gives you exactly what you need.
