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

TEST(Frame, ReadsSingleSideInformation){
  FILE * pFile;
  pFile = fopen(TEST_MUSIC"300HzSine.mp3", "r");
  if(pFile == NULL){
    fprintf(stderr, "Error opening File\n");
    FAIL();
    return;
  }
  fgetc(pFile);
  unsigned char tmpOutput[0x1FF];
  FrameHeader header;
  int start = advanceToFrameHeader(pFile, &tmpOutput[0], 0x1FF);
  populateFrameHeader(pFile, &tmpOutput[0], start, &header);
  SideInformation sideInfo = {0};
  readSingleSideInformation(pFile, &sideInfo);
  ASSERT_EQ(0, sideInfo.mainDataBegin);
  ASSERT_EQ(0, sideInfo.private_bits);
  ASSERT_EQ(0, sideInfo.scfsi);

  ASSERT_EQ(0b001101011101, sideInfo.granual1.par23Length);
  ASSERT_EQ(0b10001, sideInfo.granual1.bigValues);
  ASSERT_EQ(0b10101011, sideInfo.granual1.globalGain);
  ASSERT_EQ(0b1010, sideInfo.granual1.scalefacCompress);
  ASSERT_EQ(1, sideInfo.granual1.windowsSwitchingFlag);
  ASSERT_EQ(1, sideInfo.granual1.blockType);
  ASSERT_EQ(0, sideInfo.granual1.mixedBlockFlag);
  ASSERT_EQ(0b1100000000, sideInfo.granual1.tableSelect);
  ASSERT_EQ(0, sideInfo.granual1.subblockGain);
  ASSERT_EQ(0b1010, sideInfo.granual1.region0Count);
  ASSERT_EQ(0b11, sideInfo.granual1.region1Count);
  ASSERT_EQ(1, sideInfo.granual1.preflag);
  ASSERT_EQ(0, sideInfo.granual1.scalefacScale);
  ASSERT_EQ(1, sideInfo.granual1.count1TableSelect);
  fclose(pFile);
}
