#include <gtest/gtest.h>

#include <Eigen/Core>

#include "rtff/buffer/audio_buffer.h"
#include "rtff/buffer/ring_buffer.h"
#include "rtff/buffer/multichannel_ring_buffer.h"

TEST(Buffer, AudioBuffer) {
  // Test convertion split channel to interleaved and interleaved to split
  // channel
  using namespace rtff;
  const auto frame_number = 44100;
  const auto channel_count = 2;

  AudioBuffer buffer_from_split(frame_number, channel_count);
  AudioBuffer buffer_from_interleaved(frame_number, channel_count);

  Eigen::VectorXf left_channel = Eigen::VectorXf::Random(frame_number);
  Eigen::VectorXf right_channel = Eigen::VectorXf::Random(frame_number);
  Eigen::MatrixXf interleaved(channel_count, frame_number);
  interleaved.row(0) = left_channel;
  interleaved.row(1) = right_channel;

  // write data
  std::copy(left_channel.data(), left_channel.data() + frame_number,
            buffer_from_split.data(0));
  std::copy(right_channel.data(), right_channel.data() + frame_number,
            buffer_from_split.data(1));
  buffer_from_interleaved.fromInterleaved(interleaved.data());

  // read
  Eigen::VectorXf read_left = Eigen::Map<Eigen::VectorXf>(
      buffer_from_interleaved.data(0), frame_number);
  Eigen::VectorXf read_right = Eigen::Map<Eigen::VectorXf>(
      buffer_from_interleaved.data(1), frame_number);
  Eigen::MatrixXf read_interleaved(channel_count, frame_number);
  buffer_from_split.toInterleaved(read_interleaved.data());

  // make sure it is the same
  ASSERT_EQ(read_left, left_channel);
  ASSERT_EQ(read_right, right_channel);
  ASSERT_EQ(interleaved, read_interleaved);
}

TEST(Buffer, RingBuffer) {
  using namespace rtff;

  const auto frame_number = 44100;
  Eigen::VectorXf data = Eigen::VectorXf::Random(frame_number);

  const auto write_size = 512;  // write one block at a time
  const auto read_size = 2048;  // read fft_size data
  const auto step_size = 1024;  // overlap
  auto current_frame = 0;
  auto successful_read_count = 0;

  Eigen::VectorXf output_data(read_size);
  RingBuffer buffer(write_size, read_size, step_size);

  // write first 3 parts
  // => we shouldn't be able to read as we don't have enough data
  buffer.Write(data.segment(current_frame, write_size).data());
  ASSERT_FALSE(buffer.Read(output_data.data()));
  current_frame += write_size;
  buffer.Write(data.segment(current_frame, write_size).data());
  ASSERT_FALSE(buffer.Read(output_data.data()));
  current_frame += write_size;
  buffer.Write(data.segment(current_frame, write_size).data());
  ASSERT_FALSE(buffer.Read(output_data.data()));
  current_frame += write_size;
  // write 4th part
  // => we have enough data. outut should be the first read_size frames of the
  // input signal
  buffer.Write(data.segment(current_frame, write_size).data());
  ASSERT_TRUE(buffer.Read(output_data.data()));
  ASSERT_EQ(output_data, data.segment(0, read_size));
  current_frame += write_size;
  successful_read_count ++;
  // write 5th part
  // can't read as we don't have enough data
  buffer.Write(data.segment(current_frame, write_size).data());
  ASSERT_FALSE(buffer.Read(output_data.data()));
  current_frame += write_size;
  // write 6th part
  // => read should work and read data should be the input skipped by step_size
  buffer.Write(data.segment(current_frame, write_size).data());
  ASSERT_TRUE(buffer.Read(output_data.data()));
  ASSERT_EQ(output_data, data.segment(step_size, read_size));
  current_frame += write_size;
  successful_read_count ++;
  
  // keep on doing this until the end of the signal
  while (current_frame < frame_number - read_size) {
    // can't read the first time
    buffer.Write(data.segment(current_frame, write_size).data());
    ASSERT_FALSE(buffer.Read(output_data.data()));
    current_frame += write_size;
    
    // can read the second
    buffer.Write(data.segment(current_frame, write_size).data());
    ASSERT_TRUE(buffer.Read(output_data.data()));
    ASSERT_EQ(output_data, data.segment(successful_read_count * step_size, read_size));
    current_frame += write_size;
    successful_read_count ++;
  }
}

TEST(Buffer, MultichannelRingBuffer) {
  using namespace rtff;
  
  const auto frame_number = 44100;
  const auto channel_number = 2;
  Eigen::MatrixXf data = Eigen::MatrixXf::Random(channel_number, frame_number);
  
  const auto write_size = 512;  // write one block at a time
  const auto read_size = 2048;  // read fft_size data
  const auto step_size = 1024;  // overlap
  auto current_frame = 0;
  auto successful_read_count = 0;
  
  Eigen::MatrixXf output_data(channel_number, read_size);
  Eigen::MatrixXf input_data(channel_number, write_size);
  MultichannelRingBuffer buffer(write_size, read_size, step_size, channel_number);
  
  AudioBuffer input_buffer(write_size, channel_number);
  AudioBuffer output_buffer(read_size, channel_number);
  
  // write first 2 can't read
  input_data = data.block(0, current_frame, channel_number, write_size);
  input_buffer.fromInterleaved(input_data.data());
  buffer.Write(input_buffer);
  ASSERT_FALSE(buffer.Read(&output_buffer));
  current_frame += write_size;
  input_data = data.block(0, current_frame, channel_number, write_size);
  input_buffer.fromInterleaved(input_data.data());
  buffer.Write(input_buffer);
  ASSERT_FALSE(buffer.Read(&output_buffer));
  current_frame += write_size;
  
  // then each time we add 2 we can read 1
  while (current_frame < frame_number - read_size) {
    input_data = data.block(0, current_frame, channel_number, write_size);
    input_buffer.fromInterleaved(input_data.data());
    buffer.Write(input_buffer);
    ASSERT_FALSE(buffer.Read(&output_buffer));
    current_frame += write_size;
    
    input_data = data.block(0, current_frame, channel_number, write_size);
    input_buffer.fromInterleaved(input_data.data());
    buffer.Write(input_buffer);
    ASSERT_TRUE(buffer.Read(&output_buffer));
    output_buffer.toInterleaved(output_data.data());
    ASSERT_EQ(output_data, data.block(0, successful_read_count * step_size, channel_number, read_size));
    
    current_frame += write_size;
    successful_read_count ++;
  }
}
