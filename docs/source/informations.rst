Useful informations
===================

Latency
^^^^^^^

TODO


Fast Fourier Transform
^^^^^^^^^^^^^^^^^^^^^^

Intel Math Kernel Library (MKL)
-------------------------------

The default fft backend is the Intel Math Kernel Library. To use this, you
just need to download it from Intel's website here: https://software.intel.com/en-us/mkl


FFTW
----

If for some reason you don't want to use the MKL, we also provide a way to use
the fftw for linux and OSX.
All you need is to disable the use of the MKL in the CMake options:

.. code:: bash

  cmake -Drtff_use_mkl=OFF ..

We also provide a way to leverage the fftw wisdom principle. See http://www.fftw.org/fftw3_doc/Wisdom-Import.html#Wisdom-Import
for more information.

To enable this, add another option to the cmake command:

.. code:: bash

  cmake -Drtff_use_mkl=OFF -Drtff_fftw_use_wisdom=ON ..

The wisdom consist of a file containing informations on how to compute a faster
transform. Its generation takes a very long time (up to a couple of minutes) but
it speeds up the fft computation once initialized.

In rtff, we look for files named `rtff_[fft size].fftw` in the current folder.
If none can be found, the file will get generated so it can be loaded during the
next run.

If the intial delay isn't acceptable in your case, you can always generate the
wisdom file yourself and name it correctly in you run directory. This way you
can take advantage of the speed improvement without worring about the
initialization duration.
