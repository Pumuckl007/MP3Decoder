#include <stdio.h>
#define FRAME_HEADER_BYTE_1 0xFF
#define FRAME_HEADER_BYTE_2 0xFB

#define AUDIO_ID_MASK 0b00011000
#define AUDIO_ID_SHIFT 3
#define LAYER_DESCRIPTION_MASK 0b0000110
#define LAYER_DESCRIPTION_SHIFT 1
#define PROTECTION_BIT_MASK 0b00000001
#define PROTECTION_BIT_SHIFT 0
#define BITRATE_INDEX_MASK 0b11110000
#define BITRATE_INDEX_SHIFT 4
#define SAMPLE_RATE_INDEX_MASK 0b00001100
#define SAMPLE_RATE_INDEX_SHIFT 2
#define PADDING_BIT_MASK 0b00000010
#define PADDING_BIT_SHIFT 1
#define CHANNEL_MODE_MASK 0b11000000
#define CHANNEL_MODE_SHIFT 6

/**
 * A struct to hold the data associated with the MP3 Header.
 * see http://mpgedit.org/mpgedit/mpeg_format/mpeghdr.htm for more info.
 */
typedef struct FrameHeader {
  unsigned short audioIdVersion;
  unsigned short layerDescription;
  unsigned short protectionBit;
  unsigned short bitrateIndex;
  unsigned short sampleRateIndex;
  unsigned short paddingBit;
  unsigned short channelMode;
} FrameHeader;

/**
 * Advances a file pointer to the next frame header writing the preceding
 * data to output.
 *
 * @param pFile the file to read from
 * @param output the output data to write to
 * @param maxRead the maximum number of bytes to read
 * @return the number of bytes read or EOF
 */
int advanceToFrameHeader(FILE *pFile, unsigned char *output, int maxRead){
  int charGot;
  int bytesRead = 0;
  do {
    charGot = fgetc(pFile);
    output[bytesRead] = (unsigned char) charGot;

    if(charGot == FRAME_HEADER_BYTE_1 && ( bytesRead + 1 ) < maxRead){
      bytesRead++;
      charGot = fgetc(pFile);
      output[bytesRead] = charGot;
      if(charGot == FRAME_HEADER_BYTE_2){
        return bytesRead;
      }
    }

    bytesRead++;
  } while(charGot != EOF && bytesRead < maxRead);
  if(charGot == EOF){
    return EOF;
  }
  return bytesRead;
}

/**
 * Reads in the data associated with the mp3 frame header and saves it to the
 * header struct. The first two bytes are expected to be read into output at
 * headerStart - 1 and headerStart.
 *
 * @param pFile the mp3 file to read from
 * @param output the data read so far in the mp3 file
 * @param headerStart where byte 2 of the mp3 header is in output
 * @param frameHeader the FrameHeader to write to.
 */
int populateFrameHeader(FILE *pFile, unsigned char *output, int headerStart,
                        FrameHeader *frameHeader){
  unsigned short byte1 = (unsigned short) output[headerStart - 1];
  unsigned short byte2 = (unsigned short) output[headerStart];
  unsigned short byte3, byte4;
  int charGot = fgetc(pFile);
  if(charGot == EOF){
    return EOF;
  }
  byte3   = (unsigned short) charGot;

  charGot = fgetc(pFile);
  if(charGot == EOF){
    return EOF;
  }
  byte4   = (unsigned short) charGot;

  frameHeader->audioIdVersion   = ( AUDIO_ID_MASK & byte2 ) >> AUDIO_ID_SHIFT;
  frameHeader->layerDescription = ( LAYER_DESCRIPTION_MASK & byte2 ) >>
                                  LAYER_DESCRIPTION_SHIFT;
  frameHeader->protectionBit    = ( PROTECTION_BIT_MASK & byte2 ) >>
                                  PROTECTION_BIT_SHIFT;
  frameHeader->bitrateIndex     = ( BITRATE_INDEX_MASK & byte3 ) >>
                                  BITRATE_INDEX_SHIFT;
  frameHeader->sampleRateIndex  = ( SAMPLE_RATE_INDEX_MASK & byte3 ) >>
                                  SAMPLE_RATE_INDEX_SHIFT;
  frameHeader->paddingBit  = ( PADDING_BIT_MASK & byte3 ) >>
                             PADDING_BIT_SHIFT;
  frameHeader->channelMode = ( CHANNEL_MODE_MASK & byte4 ) >>
                             CHANNEL_MODE_SHIFT;
  return 1;
}
