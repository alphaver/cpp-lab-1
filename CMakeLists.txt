cmake_minimum_required(VERSION 2.6)

project(wordcounter)
set(LIB_SOURCES src/word_counter.cpp src/external_set.cpp)
set(LIB_HEADERS include/word_counter.cpp include/external_set.h)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

add_library(wordcounter STATIC ${LIB_SOURCES})
target_include_directories(wordcounter PUBLIC include)

add_executable(test src/test_main.cpp)
target_include_directories(test PUBLIC include)
target_link_libraries(test PUBLIC wordcounter)
