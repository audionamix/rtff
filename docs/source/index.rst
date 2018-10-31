Welcome to the Realtime Frequential Filtering Library documentation!
====================================================================

*RTFF* is a framework to design audio filters in the time frequency domain.

It takes care of the conversion of the signal to the frequency domain and back
to the time domain with nothing else to configure than the size of your fourier
transform and the overlap.

Tired of coming back to the basis when designing a complex filter ? This library
is for you !

Snippet
^^^^^^^

.. code:: cpp

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


.. toctree::
   :maxdepth: 2
   :caption: Contents:

   quickstart
   informations
   reference/index
