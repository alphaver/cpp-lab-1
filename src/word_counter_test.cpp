#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "word_counter.h"

TEST(string_tests, hardcoded_string) 
{
    static const std::string str = 
        "alpha beta gamma delta alpha mama i'm a criminal";
    static const std::int32_t answer = 8;

    std::int32_t function_answer = word_counter::unique_words_string(str);
    EXPECT_EQ(function_answer, answer);
}

TEST(string_tests, empty_string)
{
    std::int32_t function_answer = word_counter::unique_words_string("");
    EXPECT_EQ(function_answer, 0);
}

TEST(file_tests, predefined_file)
{
    static const std::string file_name = "big_30_words_here.txt";

    std::int32_t function_answer = word_counter::unique_words_file(file_name);
    EXPECT_EQ(function_answer, 30);
}

TEST(file_tests, dev_null)
{
    std::int32_t function_answer = word_counter::unique_words_file("/dev/null");
    EXPECT_EQ(function_answer, 0);
}

