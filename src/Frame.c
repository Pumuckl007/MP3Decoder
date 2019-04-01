#include <stdio.h>
#define FRAME_HEADER_BYTE_1 0xFF
#define FRAME_HEADER_BYTE_2 0xFB

/**
 * Advances a file pointer to the next frame header writing the preceding
 * data to output.
 *
 * @param pFile the file to read from
 * @param output the output data to write to
 * @param maxRead the maximum number of bytes to read
 * @return the number of bytes read or EOF
 */
int advanceToFrameHeader(FILE *pFile, char *output, int maxRead){
  int charGot;
  int bytesRead = 0;
  do {
    charGot = fgetc(pFile);
    output[bytesRead] = charGot;

    if(charGot == FRAME_HEADER_BYTE_1 && (bytesRead + 1) < maxRead){
      bytesRead++;
      charGot = fgetc(pFile);
      output[bytesRead] = charGot;
      if(charGot == FRAME_HEADER_BYTE_2){
        return bytesRead;
      }
    }
    
    bytesRead++;
  } while (charGot != EOF && bytesRead < maxRead);
  if(charGot == EOF){
    return EOF;
  }
  return bytesRead;
}
