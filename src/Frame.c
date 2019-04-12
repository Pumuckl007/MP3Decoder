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


// Side information constants
#define MAIN_DATA_BEGIN_SIZE 9
#define PRIVATE_BITS_SIZE 5
#define SCFSI_SIZE 4

// Side Info Granual constants
#define PAR2_3_LENGTH_SIZE 12
#define BIG_VALUES_SIZE 9
#define GLOBAL_GAIN_SIZE 8
#define SCALEFAC_COMPRESS_SIZE 4
#define WINDOWS_SWITCHING_FLAG_SIZE 1
#define BLOCK_TYPE_SIZE 2
#define MIXED_BLOCK_FLAG_SIZE 1
#define TABLE_SELECT_SIZE_WSF_1 10
#define TABLE_SELECT_SIZE_WSF_0 15
#define SUBBLOCK_GAIN_SIZE 9
#define REGION0_COUNT_SIZE 4
#define REGION1_COUNT_SIZE 3
#define PREFLAG_SIZE 1
#define SCALFAC_SCALE_SIZE 1
#define COUNT1TABLE_SELECT_SIZE 1
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

typedef struct SideInfoGranual {
  unsigned int par23Length;
  unsigned int bigValues;
  unsigned int globalGain;
  unsigned int scalefacCompress;
  unsigned int windowsSwitchingFlag;
  unsigned int blockType;
  unsigned int mixedBlockFlag;
  unsigned int tableSelect;
  unsigned int subblockGain;
  unsigned int region0Count;
  unsigned int region1Count;
  unsigned int preflag;
  unsigned int scalefacScale;
  unsigned int count1TableSelect;
} SideInfoGranual;

typedef struct SideInformation {
  unsigned int mainDataBegin;
  unsigned int private_bits;
  unsigned int scfsi;
  SideInfoGranual granual1;
  SideInfoGranual granual2;
} SideInformation;

typedef struct BitReader {
  unsigned char buff;
  FILE *pFile;
  int used;
} BitReader;

void initReader(BitReader *reader, FILE *pFile);
int readBits(int howMany, BitReader * bitReader);
void readSingleSideInfoGranual(BitReader *reader, SideInfoGranual *granual);

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

void readSingleSideInformation(FILE *pFile, SideInformation *sideInfo){
  BitReader reader;
  initReader(&reader, pFile);
  sideInfo->mainDataBegin = readBits(MAIN_DATA_BEGIN_SIZE, &reader);
  sideInfo->private_bits  = readBits(PRIVATE_BITS_SIZE, &reader);
  sideInfo->scfsi = readBits(SCFSI_SIZE, &reader);
  readSingleSideInfoGranual(&reader, &sideInfo->granual1);
  readSingleSideInfoGranual(&reader, &sideInfo->granual2);
}

void readSingleSideInfoGranual(BitReader *reader, SideInfoGranual *granual){
  granual->par23Length          = readBits(PAR2_3_LENGTH_SIZE, reader);
  granual->bigValues            = readBits(BIG_VALUES_SIZE, reader);
  granual->globalGain           = readBits(GLOBAL_GAIN_SIZE, reader);
  granual->scalefacCompress     = readBits(SCALEFAC_COMPRESS_SIZE, reader);
  granual->windowsSwitchingFlag = readBits(WINDOWS_SWITCHING_FLAG_SIZE, reader);
  granual->blockType            = readBits(BLOCK_TYPE_SIZE, reader);
  granual->mixedBlockFlag       = readBits(MIXED_BLOCK_FLAG_SIZE, reader);

  //Compute the table select size which is dependent on windowsSwitchingFlag
  int tableSelectSize =
    ( granual->windowsSwitchingFlag ) ? TABLE_SELECT_SIZE_WSF_1 :
    TABLE_SELECT_SIZE_WSF_0;

  granual->tableSelect       = readBits(tableSelectSize, reader);
  granual->subblockGain      = readBits(SUBBLOCK_GAIN_SIZE, reader);
  granual->region0Count      = readBits(REGION0_COUNT_SIZE, reader);
  granual->region1Count      = readBits(REGION1_COUNT_SIZE, reader);
  granual->preflag           = readBits(PREFLAG_SIZE, reader);
  granual->scalefacScale     = readBits(SCALFAC_SCALE_SIZE, reader);
  granual->count1TableSelect = readBits(COUNT1TABLE_SELECT_SIZE, reader);
}

int readBits(int howMany, BitReader * bitReader){
  if(howMany == 0){
    return 0;
  }
  if(howMany > 31){
    return -1;
  }
  int acc = 0;
  while(howMany > 0){
    if(bitReader->used >= 8){
      int charGot = fgetc(bitReader->pFile);
      if(charGot < 0){
        return charGot;
      }
      bitReader->used = 0;
      bitReader->buff = (unsigned char) charGot;
    }
    acc <<= 1;
    acc  |= ( bitReader->buff & 0x80 ) >> 7;
    bitReader->buff <<= 1;
    howMany--;
    bitReader->used++;
  }
  return acc;
}

void initReader(BitReader *reader, FILE *pFile){
  reader->buff  = 0;
  reader->used  = 9;
  reader->pFile = pFile;
}
