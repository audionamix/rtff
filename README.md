[![Build status](https://ci.appveyor.com/api/projects/status/3y6vp1bns9vasil3?svg=true)](https://ci.appveyor.com/project/Audionamix/rtff-0j7kw)
[![Build Status](https://travis-ci.org/audionamix/rtff.svg?branch=master)](https://travis-ci.org/audionamix/rtff)
[![Documentation Status](https://readthedocs.org/projects/rtff/badge/?version=latest)](https://rtff.readthedocs.io/en/latest/?badge=latest)

# Real Time Frequential Filtering

A framework to design audio filters in the time frequency domain.

## Example

```cpp
#include <iostream>

#include <Eigen/Core>

#include "rtff/filter.h"

...
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
rtff::AudioBuffer buffer(filter.block_size(), channel_number);

...
filter.ProcessBlock(&buffer);
```

## Latency

Computing the short time fourier transform implies a latency. If you want to
develop a plug-in, you need to inform the DAW about the number of frames of
latency produced by your filter.  
The `AbstractFilter::FrameLatency()` function gives you exactly what you need.

# Documentation

The documentation is based on [sphinx](http://www.sphinx-doc.org/en/master/),
[breathe](https://github.com/michaeljones/breathe) and
[readthedocs](https://readthedocs.org/)  

If you ever want to build it, we provide Dockerfile
```bash
docker build -t rtff-docs -f docs/Dockerfile .
# Generate doxygen xmls
docker run -it --rm -v$(pwd):/code rtff-docs doxygen
# make sphinx documentation
docker run -it --rm -v$(pwd):/code rtff-docs make html
```

## Cross compile

You can use the [dockcross](https://github.com/dockcross/dockcross) project to
cross compile the library.  
For example to build an `arm64` cross compiled version do:

```bash
docker run --rm dockcross/linux-arm64 > dockcross
chmod +x dockcross
./dockcross cmake -H. -Bbuild -GNinja -Drtff_use_mkl=OFF -Drtff_fftw_extra_configure_flags=--host=arm-linux-gnueabi
./dockcross ninja -Cbuild
```
Note that we disable the use of the `mkl` and force a cross compile flag on the
`fftw`.
