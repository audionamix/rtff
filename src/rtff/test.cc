#include <gtest/gtest.h>

#include <iostream>

#include "rtff/filter.h"
#include "wave/file.h"

const std::string gResourcePath(TEST_RESOURCES_PATH);

class MyFilter : public rtff::Filter {
 private:
  void ProcessTransformedBlock(rtff::FrequentialBuffer* buffer) override {
    for (uint8_t channel_idx = 0; channel_idx < buffer->channel_count();
         channel_idx++) {
      buffer->channel(channel_idx).block(20, 0, 50, 1) *= 0;
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
  auto block_size = 512;
  auto channel_number = file.channel_number();

  MyFilter filter;
  std::error_code err;
  filter.Init(block_size, channel_number, err);
  ASSERT_FALSE(err);

  rtff::AudioBuffer buffer;
  buffer.Init(block_size, channel_number);

  // For debug. From this point, the application shouldn't allocate any memory.
  Eigen::internal::set_is_malloc_allowed(false);

  // Extract each frames (add latency)
  auto multichannel_buffer_size = block_size * channel_number;

  for (uint32_t sample_idx = 0;
       sample_idx < content.size() - multichannel_buffer_size;
       sample_idx += multichannel_buffer_size) {
    float* sample_ptr = content.data() + sample_idx;
    memcpy(buffer.data(), sample_ptr,
           block_size * channel_number * sizeof(float));

    filter.ProcessBlock(&buffer);

    memcpy(sample_ptr, buffer.data(),
           block_size * channel_number * sizeof(float));

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
