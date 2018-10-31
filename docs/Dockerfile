FROM ubuntu:18.04
RUN apt-get update
RUN apt-get install -y python-dev python-pip doxygen
RUN pip install breathe sphinx_rtd_theme
RUN pip install sphinxcontrib-contentui
WORKDIR /code/docs
