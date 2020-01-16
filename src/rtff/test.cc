#include <gtest/gtest.h>

#include <iostream>

#include <Eigen/Core>

#include "rtff/abstract_filter.h"
#include "rtff/filter.h"
#include "wave/file.h"

const std::string gResourcePath(TEST_RESOURCES_PATH);

class MyFilter : public rtff::AbstractFilter {
private:
  void ProcessTransformedBlock(std::vector<std::complex<float>*> data,
                               uint32_t size) override {
    for (uint8_t channel_idx = 0; channel_idx < data.size(); channel_idx++) {
      auto buffer = Eigen::Map<Eigen::VectorXcf>(data[channel_idx], size);
      ASSERT_EQ(size, fft_size() / 2 + 1);
      buffer.block(20, 0, 50, 1) *= 0;
    }
  }
};

TEST(RTFF, Basis) {
  // Read input file content
  wave::File file;
  file.Open(gResourcePath + "/Untitled3.wav", wave::OpenMode::kIn);
  std::vector<float> content(file.frame_number() * file.channel_number());
  ASSERT_EQ(file.Read(&content), wave::Error::kNoError);

  // Initialize filter
  auto block_size = 2048;
  auto channel_number = file.channel_number();

  MyFilter filter;
  std::error_code err;
  filter.Init(channel_number, err);
  ASSERT_FALSE(err);
  filter.set_block_size(block_size);

  rtff::AudioBuffer buffer(block_size, channel_number);

  // For debug. From this point, the application shouldn't allocate any memory.
  Eigen::internal::set_is_malloc_allowed(false);

  // Extract each frames (add latency)
  auto multichannel_buffer_size = block_size * channel_number;

  for (uint32_t sample_idx = 0;
       sample_idx < content.size() - multichannel_buffer_size;
       sample_idx += multichannel_buffer_size) {
    // process the input buffer
    float* sample_ptr = content.data() + sample_idx;
    
    buffer.fromInterleaved(sample_ptr);
    filter.ProcessBlock(&buffer);
    buffer.toInterleaved(sample_ptr);

    // to write, we compensate the latency
    int output_sample_idx =
    sample_idx - (filter.FrameLatency() * channel_number);
    if (output_sample_idx < 0) {
      // begining of the file. As we create latency, the first few samples will
      // be zeros. To compensate, we just remove them
      float* output_sample_ptr = content.data();
      float* processed_sample_ptr = sample_ptr + abs(output_sample_idx);
      auto size_to_copy = block_size - filter.FrameLatency();
      memcpy(output_sample_ptr, processed_sample_ptr,
             size_to_copy * channel_number * sizeof(float));
    } else {
      // after the first few buffers, we are on general case. We just have the
      // write taking the latency into consideration
      float* output_sample_ptr = content.data() + output_sample_idx;
      memcpy(output_sample_ptr, sample_ptr,
             block_size * channel_number * sizeof(float));
    }

    // display the current status
    std::cout << round(double(sample_idx * 100) /
                       (file.frame_number() * file.channel_number()))
    << "%" << std::endl;
  }

  // For debug. From this point, the application can allocate memory
  Eigen::internal::set_is_malloc_allowed(true);

  // Write the output file content
  wave::File output;
  output.Open("/tmp/rtff_res.wav", wave::OpenMode::kOut);
  output.set_sample_rate(file.sample_rate());
  output.set_channel_number(file.channel_number());
  output.set_bits_per_sample(file.bits_per_sample());
  output.Write(content);
}

// This test makes sure that changing the block size doesn't crash the process
TEST(RTFF, ChangeBlockSize) {
  MyFilter filter;
  std::error_code err;
  auto channel_number = 1;
  filter.Init(channel_number, err);
  ASSERT_FALSE(err);

  auto block_size = 512;
  filter.set_block_size(block_size);

  auto buffer = std::make_shared<rtff::AudioBuffer>(block_size, channel_number);
  // queue 50 buffer
  for (auto index = 0; index < 50; index++) {
    memset(buffer->data(0), 0, block_size);
    filter.ProcessBlock(buffer.get());
  }
  block_size = 1024;
  filter.set_block_size(block_size);
  buffer = std::make_shared<rtff::AudioBuffer>(block_size, channel_number);
  // queue 50 other buffer
  for (auto index = 0; index < 50; index++) {
    memset(buffer->data(0), 0, block_size);
    filter.ProcessBlock(buffer.get());
  }
}

// this test makes sure that filter class syntax is functionnal
TEST(RTFF, Filter) {
  rtff::Filter filter;
  std::error_code err;
  auto channel_number = 1;
  filter.Init(channel_number, err);
  filter.execute = [](std::vector<std::complex<float>*> data, uint32_t size) {
    for (uint8_t channel_idx = 0; channel_idx < data.size(); channel_idx++) {
      auto buffer = Eigen::Map<Eigen::VectorXcf>(data[channel_idx], size);
      buffer = Eigen::VectorXcf::Random(size);
    }
  };

  ASSERT_FALSE(err);
  auto block_size = 512;
  filter.set_block_size(block_size);

  rtff::AudioBuffer buffer(block_size, channel_number);
  // queue 50 buffer
  for (auto index = 0; index < 50; index++) {
    memset(buffer.data(0), 0, block_size);
    filter.ProcessBlock(&buffer);
  }
}

uint32_t GetLatency(rtff::Filter& filter);

TEST(RTFF, Latency) {
  rtff::Filter filter;
  std::error_code err;
  filter.Init(1, err);
  ASSERT_FALSE(err);

  // case blocksize % fftsize == 0
  filter.set_block_size(512);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case block size > fft size
  filter.set_block_size(filter.fft_size() + 100);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case block_size < fft_size and blocksize % fftsize != 0
  filter.set_block_size(filter.fft_size() - 100);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));
}

TEST(RTFF, Latency4096) {
  rtff::Filter filter;
  std::error_code err;
  filter.Init(1, 4096, 4096 * 0.75, err);
  ASSERT_FALSE(err);

  // case blocksize % fftsize == 0
  filter.set_block_size(512);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case block size > fft size
  filter.set_block_size(filter.fft_size() + 100);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case block_size < fft_size and blocksize % fftsize != 0
  filter.set_block_size(filter.fft_size() - 100);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));
}

TEST(RTFF, Latency0Overlap) {
  rtff::Filter filter;
  std::error_code err;
  filter.Init(1, 4096, 0, err);
  ASSERT_FALSE(err);

  // case blocksize % fftsize == 0
  filter.set_block_size(512);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case blocksize % fftsize == 0
  filter.set_block_size(filter.fft_size());
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case block size > fft size
  filter.set_block_size(filter.fft_size() + 100);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));

  // case block_size < fft_size and blocksize % fftsize != 0
  filter.set_block_size(filter.fft_size() - 100);
  ASSERT_EQ(filter.FrameLatency(), GetLatency(filter));
}

// This class make sure processing little block size does not crash
TEST(RTFF, LittleBlockSize) {
  rtff::Filter filter;
  std::error_code err;
  auto channel_number = 1;
  filter.Init(channel_number, 2048, 2048*0.75, err);
  ASSERT_FALSE(err);
  
  filter.execute = [](std::vector<std::complex<float>*> data, uint32_t size) {
    for (uint8_t channel_idx = 0; channel_idx < data.size(); channel_idx++) {
      auto buffer = Eigen::Map<Eigen::VectorXcf>(data[channel_idx], size);
      buffer = Eigen::VectorXcf::Random(size);
    }
  };
  
  // Testing 64 bytes block
  auto block_size = 64;
  rtff::AudioBuffer buffer(block_size, channel_number);
  filter.set_block_size(block_size);
  // queue 50 buffer
  for (auto index = 0; index < 50; index++) {
    memset(buffer.data(0), 0, block_size);
    filter.ProcessBlock(&buffer);
  }

  // Testing 43 bytes block
  block_size = 43;
  buffer = rtff::AudioBuffer(block_size, channel_number);
  filter.set_block_size(block_size);
  // queue 50 buffer
  for (auto index = 0; index < 50; index++) {
    memset(buffer.data(0), 0, block_size);
    filter.ProcessBlock(&buffer);
  }
  
}

// Compute the filter latency by sending a Dirac and checking the filter output
uint32_t GetLatency(rtff::Filter& filter) {
  rtff::AudioBuffer buffer(filter.block_size(), filter.channel_count());
  auto block_size = filter.block_size();

  // generate a dirac
  auto sample_rate = 44100;
  auto pre_dirac_samples = sample_rate * 1;
  std::vector<float> content(pre_dirac_samples * 2 + 1, 0);
  content[pre_dirac_samples] = 1;

  // run data into the filter
  for (uint32_t sample_idx = 0; sample_idx < content.size() - block_size;
       sample_idx += block_size) {
    float* sample_ptr = content.data() + sample_idx;
    memcpy(buffer.data(0), sample_ptr, block_size * sizeof(float));

    filter.ProcessBlock(&buffer);

    memcpy(sample_ptr, buffer.data(0), block_size * sizeof(float));
  }
  uint32_t max_index = 0;
  Eigen::Map<const Eigen::Matrix<float, Eigen::Dynamic, 1>>(content.data(),
                                                            content.size())
  .maxCoeff(&max_index);
  auto latency = max_index - pre_dirac_samples;
  return latency;
}

