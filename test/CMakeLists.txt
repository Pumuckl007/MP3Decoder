cmake_minimum_required(VERSION 3.2)
project(MP3Decoder_tests)

include_directories(${MP3_DECODER_HEADERS_DIR} ../googletest/googletest/include/)

set(SOURCE_FILES Test.cpp)

link_directories(/usr/lib/ ../googletest/googletest/build/lib)

add_executable(MP3Decoder_tests ${SOURCE_FILES})

set_target_properties (MP3Decoder_tests PROPERTIES LINK_FLAGS "-Wall")

target_link_libraries(MP3Decoder_tests gtest pthread)


add_test (test_lib ${EXECUTABLE_OUTPUT_PATH}/test_mp3)
