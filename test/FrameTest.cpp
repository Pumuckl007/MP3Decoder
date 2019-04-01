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
  char tmpOutput[0x1FF];
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
  char tmpOutput[0x1FF];
  int start = advanceToFrameHeader(pFile, &tmpOutput[0], 0x1FF);
  ASSERT_EQ(0x1A1, start);
  fclose(pFile);
}
