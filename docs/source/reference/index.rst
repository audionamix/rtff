Reference
=========

The library main class is the `rtff::Filter` and its mother the `rtff::AbstractFilter`

.. doxygenclass:: rtff::Filter
  :members: execute

.. doxygenclass:: rtff::AbstractFilter
  :members: Init, set_block_size, ProcessBlock, FrameLatency, PrepareToPlay, ProcessTransformedBlock

For more, see

.. toctree::
    :glob:

    buffer
    filter
    fft
