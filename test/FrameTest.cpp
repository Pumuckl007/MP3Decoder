#include <gtest/gtest.h>
#include <stdio.h>
#include "Frame.c"

TEST(Frame, FindsFirstFrameInFile){
  FILE * pFile;
  pFile = fopen(TEST_MUSIC"300HzSine.mp3", "r");
  if(pFile == NULL){
    fprintf(stderr, "Error opening File\n");
    FAIL();
    return;
  }
  unsigned char tmpOutput[0x1FF];
  int start = advanceToFrameHeader(pFile, &tmpOutput[0], 0x1FF);
  ASSERT_EQ(0x1, start);
  fclose(pFile);
}

TEST(Frame, FindsSecondFrameInFile){
  FILE * pFile;
  pFile = fopen(TEST_MUSIC"300HzSine.mp3", "r");
  if(pFile == NULL){
    fprintf(stderr, "Error opening File\n");
    FAIL();
    return;
  }
  fgetc(pFile);
  unsigned char tmpOutput[0x1FF];
  int start = advanceToFrameHeader(pFile, &tmpOutput[0], 0x1FF);
  ASSERT_EQ(0x1A1, start);
  fclose(pFile);
}

TEST(Frame, PopulateFrameHeaderPopulatesFirstFrame){
  FILE * pFile;
  pFile = fopen(TEST_MUSIC"300HzSine.mp3", "r");
  if(pFile == NULL){
    fprintf(stderr, "Error opening File\n");
    FAIL();
    return;
  }
  unsigned char tmpOutput[0x1FF];
  FrameHeader header;
  int start = advanceToFrameHeader(pFile, &tmpOutput[0], 0x1FF);
  int sucess = populateFrameHeader(pFile, &tmpOutput[0], start, &header);
  ASSERT_GT(sucess, 0);
  ASSERT_EQ(0b11, header.audioIdVersion);
  ASSERT_EQ(0b01, header.layerDescription);
  ASSERT_EQ(0b1, header.protectionBit);
  ASSERT_EQ(0b1001, header.bitrateIndex);
  ASSERT_EQ(0b00, header.sampleRateIndex);
  ASSERT_EQ(0b0, header.paddingBit);
  ASSERT_EQ(0b11, header.channelMode);
  fclose(pFile);
}

TEST(Frame, BitReaderReadsBits){
  FILE * pFile;
  pFile = fopen(TEST_MUSIC"300HzSine.mp3", "r");
  if(pFile == NULL){
    fprintf(stderr, "Error opening File\n");
    FAIL();
    return;
  }
  BitReader reader;
  initReader(&reader, pFile);
  ASSERT_EQ(0b1, readBits(1, &reader));
  ASSERT_EQ(0xFF, readBits(8, &reader));
  ASSERT_EQ(-1, readBits(32, &reader));
  ASSERT_EQ(0b1111011, readBits(7, &reader));
  ASSERT_EQ(0x90, readBits(8, &reader));
  ASSERT_EQ(0xC, readBits(4, &reader));
  ASSERT_EQ(0x400, readBits(12, &reader));
  ASSERT_EQ(0x0, readBits(0, &reader));
  fclose(pFile);
}
