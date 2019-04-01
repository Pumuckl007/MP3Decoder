#include <gtest/gtest.h>
#include <stdio.h>
#include "Frame.c"

TEST(Frame, FindsFirstFrameInFile){
  FILE * pFile;
  pFile = fopen("../testMusic/300HzSine.mp3", "r");
  char tmpOutput[0xFF];
  int start = findFrameHeader(pFile, &tmpOutput[0], 0xFF);
  ASSERT_EQ(0x1A1, start);
}
