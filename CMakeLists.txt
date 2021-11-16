cmake_minimum_required(VERSION 3.0)

project(wordcounter)
set(LIB_SOURCES src/word_counter.cpp) 
set(LIB_HEADERS include/word_counter.h)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

add_library(wordcounter STATIC ${LIB_SOURCES})
target_include_directories(wordcounter PUBLIC include)

enable_testing()
add_executable(wordcounter_test src/word_counter_test.cpp)
target_link_libraries(wordcounter_test gtest_main)
target_link_libraries(wordcounter_test wordcounter)

include(GoogleTest)
gtest_discover_tests(wordcounter_test)
