cmake_minimum_required(VERSION 3.2)
project(MP3Decoder_tests)

include_directories(${MP3_DECODER_HEADERS_DIR})

set(SOURCE_FILES test_main.c)

add_executable(MP3Decoder_tests ${SOURCE_FILES})

link_directories(/usr/lib/)

set_target_properties (MP3Decoder_tests PROPERTIES LINK_FLAGS "-Wall")

target_link_libraries(MP3Decoder_tests -lgtest -lpthread)

add_test (test_lib ${EXECUTABLE_OUTPUT_PATH}/test_mp3)
