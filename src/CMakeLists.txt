cmake_minimum_required(VERSION 3.2)
project(MP3Decoder)

set(SOURCE_FILES main.c)

add_executable(MP3Decoder ${SOURCE_FILES})

install(TARGETS MP3Decoder DESTINATION ${MP3_DECODER_INSTALL_BIN_DIR})
